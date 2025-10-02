#include "Camera/VehicleCameraMode_ThirdPerson.h"

#include "LyraWheeledVehicle.h"

UVehicleCameraMode_ThirdPerson::UVehicleCameraMode_ThirdPerson()
{
	FieldOfView = 90.f;
	ViewPitchMin = -25.f;
	ViewPitchMax = 50.f;
}

FVector UVehicleCameraMode_ThirdPerson::GetPivotLocation() const
{
	AActor* Target = GetTargetActor();
	if (!Target)
	{
		return FVector::ZeroVector;
	}

	// 1) If the vehicle has a skeletal mesh and a named socket, use it.
	if (USkeletalMeshComponent* Skel = Target->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (CameraSocketName != NAME_None && Skel->DoesSocketExist(CameraSocketName))
		{
			return Skel->GetSocketLocation(CameraSocketName);
		}
	}

	// 2) Fallback: use actor bounds origin + half-height so pivot sits toward the roof/center.
	FVector Origin, Extents;
	Target->GetActorBounds(/*bOnlyCollidingComponents=*/ true, Origin, Extents);

	// Origin is the bounds center; raise pivot to top of bounds + designer offset.
	const FVector RoofPivot = Origin + FVector(0.f, 0.f, Extents.Z) + FallbackPivotOffset;
	return RoofPivot;
}

FRotator UVehicleCameraMode_ThirdPerson::GetPivotRotation() const
{
	AActor* Target = GetTargetActor();
	if (!Target)
	{
		return FRotator::ZeroRotator;
	}

	// Prefer player control rotation so the camera orbits under player control.
	if (APawn* Pawn = Cast<APawn>(Target))
	{
		if (AController* C = Pawn->GetController())
		{
			if (APlayerController* PC = Cast<APlayerController>(C))
			{
				return PC->GetControlRotation();
			}
		}
	}

	// Otherwise fall back to actor rotation.
	return Target->GetActorRotation();
}
