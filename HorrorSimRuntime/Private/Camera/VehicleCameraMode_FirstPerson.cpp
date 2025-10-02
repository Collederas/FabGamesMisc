#include "Camera/VehicleCameraMode_FirstPerson.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

UVehicleCameraMode_FirstPerson::UVehicleCameraMode_FirstPerson()
{
	FirstPersonCameraOffset = FVector(30.0f, 0, 50.0f);
	ViewPitchMin = -70.0f;
	ViewPitchMax = 60.0f;
	RelativeYawMin = -90.0f;
	RelativeYawMax = 90.0f;
}

void UVehicleCameraMode_FirstPerson::OnActivation()
{
	if (AActor* Target = GetTargetActor())
	{
		LastFrameVehicleRotation = Target->GetActorRotation();
	}
	Super::OnActivation();
}

void UVehicleCameraMode_FirstPerson::UpdateView(float DeltaTime)
{
	AActor* TargetActor = GetTargetActor();
	if (!TargetActor)
	{
		return;
	}

	APawn* TargetPawn = Cast<APawn>(TargetActor);
	APlayerController* PC = TargetPawn ? TargetPawn->GetController<APlayerController>() : nullptr;
	if (!PC)
	{
		return;
	}

	// 1. Get current vehicle rotation and calculate the delta from the last frame
	const FRotator CurrentVehicleRotation = TargetActor->GetActorRotation();
	const FRotator VehicleRotationDelta = (CurrentVehicleRotation - LastFrameVehicleRotation).GetNormalized();

	// 2. Get the current ControlRotation, which only contains player mouse input from this frame.
	const FRotator PlayerControlRotation = PC->GetControlRotation();

	// 3. Combine them: The new ControlRotation is the old one plus the vehicle's change.
	// This effectively makes the camera rotate along with the vehicle.
	FRotator NewControlRotation = PlayerControlRotation + VehicleRotationDelta;
	NewControlRotation.Normalize();

	// 4. Set the final view rotation.
	View.Rotation = NewControlRotation;

	USkeletalMeshComponent* VehicleMesh = TargetActor->FindComponentByClass<USkeletalMeshComponent>();

	if (VehicleMesh && VehicleMesh->DoesSocketExist(CameraSocketName))
	{
		View.Location = VehicleMesh->GetSocketLocation(CameraSocketName);
	}
	else
	{
		// Fallback to the actor's location if the socket or mesh isn't found.
		View.Location = TargetActor->GetActorLocation();
	}
	
	// 6. CRITICAL STEP: Update View.ControlRotation.
	// The ULyraCameraComponent will use this value to set the PlayerController's
	// control rotation after this function finishes. This keeps everything in sync for the next frame.
	View.ControlRotation = View.Rotation;

	View.FieldOfView = FieldOfView;

	// 7. Store the current vehicle rotation for the next frame's calculation.
	LastFrameVehicleRotation = CurrentVehicleRotation;
}


FVector UVehicleCameraMode_FirstPerson::GetPivotLocation() const
{
	const AActor* Target = GetTargetActor();
	if (!Target)
	{
		return FVector::ZeroVector;
	}

	// Find the socket on the vehicle's skeletal mesh.
	if (const USkeletalMeshComponent* SkelMesh = Target->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (CameraSocketName != NAME_None && SkelMesh->DoesSocketExist(CameraSocketName))
		{
			// Return the world-space location of the socket.
			return SkelMesh->GetSocketLocation(CameraSocketName);
		}
	}

	// If the socket doesn't exist, fall back to the vehicle's main location.
	return Target->GetActorLocation();
}

FRotator UVehicleCameraMode_FirstPerson::GetPivotRotation() const
{
	const AActor* Target = GetTargetActor();
	if (!Target)
	{
		return FRotator::ZeroRotator;
	}

	if (const APawn* TargetPawn = Cast<APawn>(Target))
	{
		if (AController* Controller = TargetPawn->GetController())
		{
			FRotator PawnRotation = TargetPawn->GetActorRotation();
		}
	}

	return Target->GetActorRotation();
}
