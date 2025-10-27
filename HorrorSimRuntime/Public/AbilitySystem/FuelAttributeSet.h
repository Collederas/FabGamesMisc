#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "FuelAttributeSet.generated.h"


UCLASS(BlueprintType)
class HORRORSIMRUNTIME_API UFuelAttributeSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:
	UFuelAttributeSet();

	ATTRIBUTE_ACCESSORS(UFuelAttributeSet, Fuel)
	ATTRIBUTE_ACCESSORS(UFuelAttributeSet, MaxFuel);
	
	// Delegate when fuel changes
	mutable FLyraAttributeEvent OnHealthChanged;
	
	// Delegate to broadcast when the fuel attribute reaches zero
	mutable FLyraAttributeEvent OnOutOfFuel;
protected:

	UFUNCTION()
	void OnRep_Fuel(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxFuel(const FGameplayAttributeData& OldValue);

private:
	/** Fuel Attribute */
	UPROPERTY(BlueprintReadOnly, Category="HorrorSim|Vehicle", ReplicatedUsing=OnRep_Fuel, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Fuel;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxFuel, Category = "HorroSim|Vehicle", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxFuel;
	
};
