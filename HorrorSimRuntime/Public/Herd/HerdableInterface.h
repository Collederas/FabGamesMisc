
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HerdableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UHerdableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for any actor that can be controlled by an AHerdManager.
 * This allows the manager to get per-type settings, like custom movement speeds.
 */
class HORRORSIMRUNTIME_API IHerdableInterface
{
	GENERATED_BODY()

public:
	/**
	 * @return The desired MaxWalkSpeed.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Herd")
	float GetHerdSpeed() const;
};
