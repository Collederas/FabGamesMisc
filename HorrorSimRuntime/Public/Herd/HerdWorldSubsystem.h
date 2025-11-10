#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HerdWorldSubsystem.generated.h"

class AHerdManager;

UCLASS()
class HORRORSIMRUNTIME_API UHerdWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	// Called by each actor that wants to be part of the system
	UFUNCTION(BlueprintCallable, Category = "Herd")
	void RegisterHerdActor(AActor* Actor);

	// Should be called on death
	void UnregisterHerdActor(AActor* Actor);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// The main function that runs on a timer to check for new herds
	void FindAndFormHerds();

private:
	// Cache of all actors that are close enough for the herd to be formed
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> AvailableActors;
	
	FTimerHandle HerdCheckTimer;
};
