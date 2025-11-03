#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "HerdManagerSettings.generated.h"

class AHerdManager;

UCLASS(Config = Game, DefaultConfig, Meta = (DisplayName = "Herd Settings"))
class HORRORSIMRUNTIME_API UHerdManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UHerdManagerSettings();
	
	// The Blueprint class of the manager to spawn.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Herding")
	TSubclassOf<AHerdManager> HerdManagerClass;

	// How close actors must be to form a herd (in cm)
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Herding")
	float HerdFormationRadius;

	// How many actors make a herd
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Herding")
	int32 MinHerdSize;
};
