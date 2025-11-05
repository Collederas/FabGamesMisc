#include "Herd/HerdManager.h"

#include "AIController.h"
#include "Components/StateTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Navigation/PathFollowingComponent.h"

AHerdManager::AHerdManager(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	// Tick after physics to get the final, stable manager pose for members to follow.
	PrimaryActorTick.TickGroup = TG_PostPhysics; 

	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTree"));

	HerdBoundsCollider = CreateDefaultSubobject<USphereComponent>(TEXT("HerdBoundsCollider"));
	HerdBoundsCollider->SetupAttachment(GetRootComponent());
	HerdBoundsCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HerdBoundsCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
}

void AHerdManager::InitializeHerd(const TArray<AActor*>& Members)
{
	UE_LOG(LogTemp, Warning, TEXT("HERD FORMED with %d members!"), Members.Num());
	
	HerdMembersData.Empty();

	// Initialize trace params and ignore the manager itself.
	HerdTraceParams = FCollisionQueryParams();
	HerdTraceParams.bTraceComplex = false;
	HerdTraceParams.bReturnPhysicalMaterial = false;
	HerdTraceParams.AddIgnoredActor(this);

	const TArray<FVector> AllFormationOffsets = FHerdFormationLogic::GenerateFormationOffsets(
		FormationType,
		Members.Num(),
		FormationSpacing 
	);

	for (int32 i = 0; i < Members.Num(); ++i)
	{
		if (!AllFormationOffsets.IsValidIndex(i)) continue;

		AActor* Member = Members[i];
		if (!Member) continue;
		
		APawn* HerdPawn = Cast<APawn>(Members[i]);
		if (!HerdPawn) continue;
	
		FHerdMemberData Data;
		Data.Member = Member;
		Data.FormationOffset = AllFormationOffsets[i];
		Data.ZOffset = 0.f;
		
		// Try to get capsule half-height for Z-offsetting.
		if (ACharacter* Character = Cast<ACharacter>(Member))
		{
			if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
			{
				Data.ZOffset = Capsule->GetScaledCapsuleHalfHeight();
			}
		}
		else if (UCapsuleComponent* RootCapsule = Cast<UCapsuleComponent>(Member->GetRootComponent()))
		{
			Data.ZOffset = RootCapsule->GetScaledCapsuleHalfHeight();
		}

		// Store original state before modifying.
		Data.bOriginalCollisionState = Member->GetActorEnableCollision();
		HerdTraceParams.AddIgnoredActor(Member); // Add member to trace ignore list.

		if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(HerdPawn->GetMovementComponent()))
		{
			Data.OriginalMovementMode = MoveComp->MovementMode;
			Data.OriginalMaxWalkSpeed = MoveComp->MaxWalkSpeed;
			
			// Stop all movement and disable standard movement logic.
			MoveComp->StopMovementImmediately();
			MoveComp->SetMovementMode(EMovementMode::MOVE_None);
		}
		
		Member->SetActorEnableCollision(false); // Disable collision to prevent snagging.
		HerdMembersData.Add(Data);
	}

	if (StateTreeComponent)
	{
		// Start the herd brain!
		StateTreeComponent->StartLogic();
	}
}

void AHerdManager::OrderHerdToMove(const FVector& GroupTargetLocation)
{
	if (AAIController* Ctrl = GetHerdController())
	{
		// We DO NOT set a rotation here. We let the AI's path and velocity control rotation.
		
		DrawDebugSphere(GetWorld(), GroupTargetLocation, PathPointArrivalRadius, 10, FColor::Red, false, 2.f);
		// Command the AI controller to move.
		Ctrl->MoveToLocation(GroupTargetLocation, PathPointArrivalRadius);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HerdManager has no AAIController to move with!"));
	}
}

void AHerdManager::StopHerdMovement() const
{
	if (AAIController* Ctrl = GetHerdController())
	{
		Ctrl->StopMovement();
	}
}

bool AHerdManager::IsHerdMoveComplete() const
{
	if (const AAIController* Ctrl = GetHerdController())
	{
		// Check the path following component status.
		return Ctrl->GetMoveStatus() != EPathFollowingStatus::Type::Moving;
	}
	
	// Default to 'complete' if no controller.
	return true;
}

void AHerdManager::DisbandHerd()
{
	// Restore all members to their original state.
	for (const FHerdMemberData& Data : HerdMembersData)
	{
		if (AActor* Member = Data.Member.Get())
		{
			// Restore collision.
			Member->SetActorEnableCollision(Data.bOriginalCollisionState);

			// Restore movement component state.
			if (APawn* Pawn = Cast<APawn>(Member))
			{
				if (UCharacterMovementComponent* MoveComp = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent()))
				{
					MoveComp->SetMovementMode(Data.OriginalMovementMode);
					MoveComp->MaxWalkSpeed = Data.OriginalMaxWalkSpeed;
				}
			}
		}
	}

	// Clear all runtime data.
	HerdMembersData.Empty();
	HerdTraceParams = FCollisionQueryParams(); 

	if (StateTreeComponent)
	{
		StateTreeComponent->StopLogic(TEXT("Herd Disbanded"));
	}
}

void AHerdManager::SetHerdSpeed(float Speed)
{
	if (UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		CMC->MaxWalkSpeed = Speed;
	}
}

void AHerdManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateManagerRotation(DeltaSeconds);
	UpdateHerdMembers(DeltaSeconds);
	UpdateHerdBounds();
}

void AHerdManager::UpdateManagerRotation(float DeltaSeconds)
{
	if (const UCharacterMovementComponent* CMC = GetCharacterMovement())
	{
		const FVector CurrentVelocity = CMC->Velocity;

		// Only update rotation if we are actively moving.
		if (!CurrentVelocity.IsNearlyZero(0.1f))
		{
			FRotator VelocityLookRotation = FRotationMatrix::MakeFromX(CurrentVelocity).Rotator();
			VelocityLookRotation.Pitch = 0;
			VelocityLookRotation.Roll = 0; 
			
			// Smoothly interpolate to the target rotation.
			const FRotator CurrentRotation = GetActorRotation();
			const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, VelocityLookRotation, DeltaSeconds, ManagerRotationInterpolationSpeed); 
			
			SetActorRotation(NewRotation);
		}
		// If velocity is zero, we do nothing. The manager keeps its last rotation.
	}
}

void AHerdManager::UpdateHerdMembers(float DeltaSeconds)
{
	if (HerdMembersData.Num() == 0)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World) 
	{
		return;
	}
	
	const FVector ManagerLocation = GetActorLocation();
	const FRotator ManagerRotation = GetActorRotation();

	for (const FHerdMemberData& Data : HerdMembersData)
	{
		AActor* Member = Data.Member.Get();
		if (!Member) continue;

		const FVector WorldOffset = ManagerRotation.RotateVector(Data.FormationOffset);
		FVector TargetLocation = ManagerLocation + WorldOffset;
		// The member should face the same way as the manager.
		const FRotator TargetRotation = ManagerRotation;
		
		// Perform a ground trace to place the member on the ground.
		const FVector TraceStart = TargetLocation + FVector(0, 0, GroundCheckTraceDistance * 0.5f);
		const FVector TraceEnd = TargetLocation - FVector(0, 0, GroundCheckTraceDistance);
		
		FHitResult Hit;
		float TargetZ = TargetLocation.Z; // Default to manager's Z if no ground found
		if (World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, HerdTraceParams))
		{
			// Apply the Z-offset to position the capsule bottom at the hit location.
			TargetZ = Hit.Location.Z + Data.ZOffset; 
		}
		const FVector CurrentLocation = Member->GetActorLocation();
		const FRotator CurrentRotation = Member->GetActorRotation();
		FVector NewLocation = FVector(TargetLocation.X, TargetLocation.Y, CurrentLocation.Z);

		NewLocation.Z = FMath::FInterpTo(CurrentLocation.Z, TargetZ, DeltaSeconds, MemberInterpolationSpeed);

		const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, MemberRotationInterpolationSpeed);

		Member->SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}


void AHerdManager::UpdateHerdBounds()
{
	// Don't do anything if the collider or herd doesn't exist.
	if (!HerdBoundsCollider || HerdMembersData.Num() == 0)
	{
		return;
	}

	// Create a bounding box that will be expanded to fit all members.
	FBox HerdBox(ForceInit); 

	for (const FHerdMemberData& Data : HerdMembersData)
	{
		if (AActor* Member = Data.Member.Get())
		{
			// Expand the bounding box to include this member's *actual, interpolated* location.
			HerdBox += Member->GetActorLocation();
		}
	}

	// Only update if the box is valid (i.e., it contains at least one point).
	if (HerdBox.IsValid)
	{
		const FVector HerdCenter = HerdBox.GetCenter();
		
		// Calculate the radius needed to encompass the farthest member.
		// We use the box's "Extent" (half-size) and get its diagonal length (.Size()),
		// which guarantees the sphere contains the box's farthest corners.
		const float HerdRadius = HerdBox.GetExtent().Size();

		// Set the sphere's final position and radius for this frame.
		HerdBoundsCollider->SetWorldLocation(HerdCenter);
		HerdBoundsCollider->SetSphereRadius(HerdRadius);
	}
}

AAIController* AHerdManager::GetHerdController() const
{
	return Cast<AAIController>(GetController());
}