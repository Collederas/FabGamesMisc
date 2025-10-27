#include "AbilitySystem/ActorWithAbilities_HorrorSimProp.h"

#include "Components/SphereComponent.h"
#include "UI/IndicatorSystem/IndicatorDescriptor.h"
#include "UI/IndicatorSystem/LyraIndicatorManagerComponent.h"


AActorWithAbilities_HorrorSimProp::AActorWithAbilities_HorrorSimProp(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	RootComponent = CollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionVolume"));
	CollisionVolume->InitSphereRadius(80.f);
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &AActorWithAbilities_HorrorSimProp::OnOverlapBegin);
	CollisionVolume->OnComponentEndOverlap.AddDynamic(this, &AActorWithAbilities_HorrorSimProp::OnOverlapEnd);
}

ULyraIndicatorManagerComponent* AActorWithAbilities_HorrorSimProp::GetLocalIndicatorManager(AActor* TargetActor)
{
	if (APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		if (APlayerController* PC = Cast<APlayerController>(TargetPawn->GetController()))
		{
			if (PC->IsLocalController())
			{
				return ULyraIndicatorManagerComponent::GetComponent(PC);
			}
		}
	}
	return nullptr;
}

void AActorWithAbilities_HorrorSimProp::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepHitResult)
{
	if (AttributePairsToMonitor.IsEmpty())
	{
		return;
	}

	if (ULyraIndicatorManagerComponent* IndicatorManager = GetLocalIndicatorManager(OtherActor))
	{
		APlayerController* PC = Cast<APlayerController>(IndicatorManager->GetOwner());

		// Don't add another indicator if this player already has one for this prop
		if (ActiveIndicators.Contains(PC))
		{
			return;
		}

		UIndicatorDescriptor* Indicator = NewObject<UIndicatorDescriptor>();

		// Create and set the data object
		UIndicatorData_AttributeMonitor* IndicatorDataObject = NewObject<UIndicatorData_AttributeMonitor>(Indicator);
		IndicatorDataObject->TargetActor = this;
		IndicatorDataObject->AttributePairsToMonitor = AttributePairsToMonitor;
		Indicator->SetDataObject(IndicatorDataObject);
        
		Indicator->SetSceneComponent(GetRootComponent());
		Indicator->SetIndicatorClass(DefaultAttributeIndicatorWidgetClass);
        
		IndicatorManager->AddIndicator(Indicator);

		// Keep track of it
		ActiveIndicators.Add(PC, Indicator);	
    }
}


void AActorWithAbilities_HorrorSimProp::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ULyraIndicatorManagerComponent* IndicatorManager = GetLocalIndicatorManager(OtherActor))
	{
		APlayerController* PC = Cast<APlayerController>(IndicatorManager->GetOwner());

		// Find the indicator associated with this player
		if (UIndicatorDescriptor* Indicator = ActiveIndicators.FindRef(PC))
		{
			IndicatorManager->RemoveIndicator(Indicator);
			ActiveIndicators.Remove(PC);
		}
	}
}