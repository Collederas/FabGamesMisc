#include "Herd/HerdManager.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "NavigationSystem.h"
#include "Components/StateTreeComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Herd/HerdableInterface.h"
#include "Navigation/PathFollowingComponent.h"


AHerdManager::AHerdManager()
{
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
}

void AHerdManager::InitializeHerd(const TArray<AActor*>& HerdMembers)
{

	UE_LOG(LogTemp, Warning, TEXT("HERD FORMED with %d members!"), HerdMembers.Num());

	for (AActor* Member : HerdMembers)
	{
		APawn* HerdChar = Cast<APawn>(Member);
		if (!HerdChar) continue;

		AAIController* PawnController = Cast<AAIController>(HerdChar->GetController());
		if (PawnController)
		{
			if (UNavMovementComponent* MoveComp = HerdChar->GetMovementComponent())
			{
				OriginalActorSpeeds.Add(PawnController, MoveComp->GetMaxSpeed());
			}
			
			PawnController->GetBrainComponent()->StopLogic("HerdTakeover");

			// Add to our list for later
			HerdMemberControllers.Add(PawnController);
		}
	}

	// Now that the herd is formed and all members are "zombies",
	// we would start the group brain.
	StateTreeComponent->StartLogic();
}

void AHerdManager::OrderHerdToMove(FVector TargetLocation)
{
	// TODO: move to settings!
	const float HerdSpreadRadius = 250.0f;
	const float HerdSpeed = 100.0f;

	// Get the navigation system to find reachable points
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("HerdManager::OrderHerdToMove: Cannot find Navigation System!"));
		// We can still proceed, but all actors will go to the exact target
	}
	for (const TWeakObjectPtr<AAIController>& ControllerPtr : HerdMemberControllers)
	{
		if (AAIController* Controller = ControllerPtr.Get())
		{
			ACharacter* HerdChar = Cast<ACharacter>(Controller->GetPawn());
			if (!HerdChar) continue;

			UCharacterMovementComponent* MoveComp = HerdChar->GetCharacterMovement();
			if (!MoveComp) continue;

			float NewHerdSpeed = MoveComp->GetMaxSpeed(); // Default to current

			if (HerdChar->GetClass()->ImplementsInterface(UHerdableInterface::StaticClass()))
			{
				NewHerdSpeed = IHerdableInterface::Execute_GetHerdSpeed(HerdChar);
			}
			FVector FinalTarget = TargetLocation; // Default to the exact spot

			if (NavSys)
			{
				FVector2D RandomOffset = FMath::RandPointInCircle(HerdSpreadRadius);
				FVector IndividualTarget = TargetLocation + FVector(RandomOffset.X, RandomOffset.Y, 0);

				// Find the closest reachable point on the navmesh to our random spot
				FNavLocation NavigablePoint;
				if (NavSys->GetRandomPointInNavigableRadius(IndividualTarget, 50.0f, NavigablePoint))
				{
					FinalTarget = NavigablePoint.Location;
				}
				// If it fails, FinalTarget just remains the original TargetLocation (a safe fallback)
			}
		
			MoveComp->MaxWalkSpeed = NewHerdSpeed;
			Controller->MoveToLocation(TargetLocation, HerdSpeed);
		}
	}
}

void AHerdManager::StopHerdMovement()
{
	for (const TWeakObjectPtr<AAIController>& ControllerPtr : HerdMemberControllers)
	{
		AAIController* Controller = ControllerPtr.Get();
		if (!Controller) continue;
	
		Controller->StopMovement();

		ACharacter* HerdChar = Cast<ACharacter>(Controller->GetPawn());
		if (HerdChar)
		{
			if (UCharacterMovementComponent* MoveComp = HerdChar->GetCharacterMovement())
			{
				// Find the original speed we saved and restore it
				if (const float* OriginalSpeed = OriginalActorSpeeds.Find(ControllerPtr))
				{
					MoveComp->MaxWalkSpeed = *OriginalSpeed;
				}
			}
		}
	}
}

bool AHerdManager::IsHerdMoveComplete() const
{
	if (HerdMemberControllers.Num() == 0)
	{
		return true; // No members, so the "move" is instantly complete.
	}

	for (const TWeakObjectPtr<AAIController>& ControllerPtr : HerdMemberControllers)
	{
		if (AAIController* Controller = ControllerPtr.Get())
		{
			if (UPathFollowingComponent* PathComp = Controller->GetPathFollowingComponent())
			{
				// If *any* member is still actively moving, the task is not done.
				if (PathComp->GetStatus() == EPathFollowingStatus::Moving)
				{
					return false;
				}
			}
		}
	}

	// If we got through the whole loop and *no one* was still "Moving",
	// then the task is complete.
	return true;
}

