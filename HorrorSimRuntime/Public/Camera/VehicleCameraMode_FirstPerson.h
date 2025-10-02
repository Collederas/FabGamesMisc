#pragma once

#include "Camera/LyraCameraMode_ThirdPerson.h"
#include "VehicleCameraMode_FirstPerson.generated.h"

/**
 * A camera mode for a vehicle's first-person view, locked to a socket.
 */
UCLASS(Blueprintable)
class HORRORSIMRUNTIME_API UVehicleCameraMode_FirstPerson : public ULyraCameraMode
{
	GENERATED_BODY()

public:
	UVehicleCameraMode_FirstPerson();

	virtual void OnActivation() override;
protected:
	virtual void UpdateView(float DeltaTime) override;
	
	// Overrides the base implementation to provide a specific location.
	virtual FVector GetPivotLocation() const override;

	// Overrides the base implementation to provide a specific rotation.
	virtual FRotator GetPivotRotation() const override;

	UPROPERTY(EditDefaultsOnly, Category = "First Person")
	FName CameraSocketName = TEXT("FirstPersonCam");

	UPROPERTY(EditDefaultsOnly, Category = "First Person")
	FVector FirstPersonCameraOffset;

	UPROPERTY(EditDefaultsOnly, Category = "First Person")
	float RelativeYawMin;

	UPROPERTY(EditDefaultsOnly, Category = "First Person")
	float RelativeYawMax;

private:
	FRotator LastFrameVehicleRotation;
};

