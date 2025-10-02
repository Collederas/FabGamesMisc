#pragma once

#include "Camera/LyraCameraMode_ThirdPerson.h"
#include "VehicleCameraMode_ThirdPerson.generated.h"

UCLASS()
class UVehicleCameraMode_ThirdPerson : public ULyraCameraMode_ThirdPerson
{
	GENERATED_BODY()

public:
	UVehicleCameraMode_ThirdPerson();

	UPROPERTY(EditDefaultsOnly, Category = "Lyra | Vehicle")
	FName CameraSocketName = TEXT("ThirdPersonCam");

	/** Fallback offset applied on top of bounds-based pivot). */
	UPROPERTY(EditDefaultsOnly, Category = "Lyra | Vehicle")
	FVector FallbackPivotOffset = FVector(0.f, 0.f, 80.f);

public:
	virtual FVector GetPivotLocation() const override;
	virtual FRotator GetPivotRotation() const override;
};
