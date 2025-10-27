#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/ActorWithAbilities.h"
#include "UI/IndicatorSystem/LyraIndicatorManagerComponent.h"
#include "ActorWithAbilities_HorrorSimProp.generated.h"

class USphereComponent;
class UCapsuleComponent;
class UIndicatorDescriptor;

/**
 * A struct to define a pair of attributes for monitoring,
 * typically a "Current" value and a "Max" value.
 */
USTRUCT(BlueprintType)
struct FMonitoredAttributePair
{
	GENERATED_BODY()

	/** The main attribute to display (e.g., Health) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute CurrentAttribute;

	/** * The attribute to use as the denominator (e.g., MaxHealth). 
	 * If this is invalid, the widget will just show the CurrentAttribute's value.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute MaxAttribute;
};


UCLASS(BlueprintType)
class HORRORSIMRUNTIME_API UIndicatorData_AttributeMonitor : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Indicator Data")
	TWeakObjectPtr<AActor> TargetActor;

	/** This is the new array of attribute pairs */
	UPROPERTY(BlueprintReadOnly, Category = "Indicator Data")
	TArray<FMonitoredAttributePair> AttributePairsToMonitor;
};

UCLASS()
class HORRORSIMRUNTIME_API AActorWithAbilities_HorrorSimProp : public AActorWithAbilities
{
	GENERATED_BODY()

public:
	AActorWithAbilities_HorrorSimProp(const FObjectInitializer& ObjectInitializer);
	
	static ULyraIndicatorManagerComponent* GetLocalIndicatorManager(AActor* TargetActor);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                    const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Indicator")
	TSoftClassPtr<UUserWidget> DefaultAttributeIndicatorWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Indicator")
	TArray<FMonitoredAttributePair> AttributePairsToMonitor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> CollisionVolume;

	UPROPERTY()
	TMap<TObjectPtr<APlayerController>, TObjectPtr<UIndicatorDescriptor>> ActiveIndicators;

};
