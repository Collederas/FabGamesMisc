#include "Camera/VehicleCameraMode_FirstPerson.h"

#include "LyraWheeledVehicle.h"
#include "Character/LyraCharacter.h"
#include "GameFramework/Actor.h"

UVehicleCameraMode_FirstPerson::UVehicleCameraMode_FirstPerson()
{
	ViewPitchMin = -70.0f;
	ViewPitchMax = 60.0f;
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
	Super::UpdateView(DeltaTime);

	// The only task left for our override is to update the state for the next frame.
	if (const AActor* TargetActor = GetTargetActor())
	{
		LastFrameVehicleRotation = TargetActor->GetActorRotation();
	}
}


FVector UVehicleCameraMode_FirstPerson::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	if (!TargetActor)
	{
		return FVector::ZeroVector;
	}

	const ALyraWheeledVehicle* Vehicle = Cast<ALyraWheeledVehicle>(TargetActor);
	ALyraCharacter* Driver = Vehicle->GetDriver();
	
	
	// A generic way to find a mesh:
	const UMeshComponent* TargetMesh = Vehicle->GetMesh();

	if (TargetMesh && TargetMesh->DoesSocketExist(CameraSocketName))
	{
		return TargetMesh->GetSocketLocation(CameraSocketName);
	}
    
	// Fallback to the actor's location if the socket or mesh isn't found.
	return TargetActor->GetActorLocation();
}

FRotator UVehicleCameraMode_FirstPerson::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	if (!TargetActor)
	{
		return FRotator::ZeroRotator;
	}

	const APawn* TargetPawn = Cast<APawn>(TargetActor);
	const APlayerController* PC = TargetPawn ? TargetPawn->GetController<APlayerController>() : nullptr;
	if (!PC)
	{
		return TargetActor->GetActorRotation();
	}

	const FRotator CurrentVehicleRotation = TargetActor->GetActorRotation();
	const FRotator VehicleRotationDelta = (CurrentVehicleRotation - LastFrameVehicleRotation).GetNormalized();
	const FRotator PlayerControlRotation = PC->GetControlRotation();
    
	FRotator NewControlRotation = PlayerControlRotation + VehicleRotationDelta;
	NewControlRotation.Normalize();

	return NewControlRotation;
}
