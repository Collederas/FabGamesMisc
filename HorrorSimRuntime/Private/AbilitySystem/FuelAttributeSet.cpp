
#include "AbilitySystem/FuelAttributeSet.h"

#include "Net/UnrealNetwork.h"

UFuelAttributeSet::UFuelAttributeSet()
:Fuel(200.0f), MaxFuel(200.0f)
{
}

void UFuelAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFuelAttributeSet, Fuel, COND_OwnerOnly, REPNOTIFY_Always);
}

void UFuelAttributeSet::OnRep_Fuel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFuelAttributeSet, Fuel, OldValue);
	
	const float CurrentFuel = GetFuel();
	const float EstimatedMagnitude = CurrentFuel - OldValue.GetCurrentValue();
	
	if (CurrentFuel <= 0.0f)
	{
		// Broadcast event to listeners (like ALyraWheeledVehicle)
		OnOutOfFuel.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentFuel);
	}
}

void UFuelAttributeSet::OnRep_MaxFuel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFuelAttributeSet, MaxFuel, OldValue);
}

