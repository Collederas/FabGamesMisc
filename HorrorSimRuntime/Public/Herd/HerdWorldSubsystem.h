#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "HerdWorldSubsystem.generated.h"

class AHerdManager;
/**
 * 
 */
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
	// This is where we'll set the timer to run our check
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// The main function that runs on a timer to check for new herds
	void FindAndFormHerds();

private:
	// This is our simple spatial partition. Just a list of all cows NOT in a herd.
	// TWeakObjectPtr is safer than raw pointers, prevents crashes if a cow is destroyed.
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> AvailableActors;
	
	// Timer handle for our check
	FTimerHandle HerdCheckTimer;
};
