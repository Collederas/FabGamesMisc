#include "Herd/HerdWorldSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Herd/HerdManager.h"
#include "Herd/HerdManagerSettings.h"


void UHerdWorldSubsystem::RegisterHerdActor(AActor* HerdActor)
{
    if (HerdActor)
    {
        AvailableActors.Add(HerdActor);
    }
}

void UHerdWorldSubsystem::UnregisterHerdActor(AActor* HerdActor)
{
    if (HerdActor)
    {
        AvailableActors.Remove(HerdActor);
    }
}

void UHerdWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    GetWorld()->GetTimerManager().SetTimer(
        HerdCheckTimer, 
        this, 
        &UHerdWorldSubsystem::FindAndFormHerds, 
        1.5f, 
        true
    );
}

void UHerdWorldSubsystem::FindAndFormHerds()
{
    const UHerdManagerSettings* HerdSettings = GetDefault<UHerdManagerSettings>();
    if (!HerdSettings)
    {
        UE_LOG(LogTemp, Error, TEXT("HerdWorldSubsystem: Cannot find HerdSettings! Check Project Settings."));
        return;
    }

    if (!HerdSettings->HerdManagerClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("HerdWorldSubsystem: HerdManagerClass is not set in Project Settings!"));
        return;
    }
    
    if (AvailableActors.Num() < HerdSettings->MinHerdSize)
    {
        return;
    }

    TSet<AActor*> HerdedHerdActors;

    for (int32 i = 0; i < AvailableActors.Num(); ++i)
    {
        AActor* HerdActorA = AvailableActors[i].Get();
        if (!HerdActorA || HerdedHerdActors.Contains(HerdActorA))
        {
            continue; // HerdActor is dead or already herded
        }

        // This is a potential herd founder. Let's find its friends.
        TArray<AActor*> NewHerdGroup;
        NewHerdGroup.Add(HerdActorA);
        
        for (int32 j = i + 1; j < AvailableActors.Num(); ++j)
        {
            AActor* HerdActorB = AvailableActors[j].Get();
            if (!HerdActorB || HerdedHerdActors.Contains(HerdActorB))
            {
                continue;
            }

            if (FVector::Dist(HerdActorA->GetActorLocation(), HerdActorB->GetActorLocation()) <= HerdSettings->HerdFormationRadius)
            {
                NewHerdGroup.Add(HerdActorB);
            }
        }

        // Did we find a big enough group?
        if (NewHerdGroup.Num() >= HerdSettings->MinHerdSize)
        {
            // Create the Herd
            FVector SpawnLocation = NewHerdGroup[0]->GetActorLocation() + FVector(0, 0, 100);
            AHerdManager* NewManager = GetWorld()->SpawnActor<AHerdManager>(
                HerdSettings->HerdManagerClass, 
                SpawnLocation, 
                FRotator::ZeroRotator
            );

            if (NewManager)
            {
                NewManager->InitializeHerd(NewHerdGroup);

                // Mark these HerdActors as "herded" so we don't check them again
                for (AActor* HerdedHerdActor : NewHerdGroup)
                {
                    HerdedHerdActors.Add(HerdedHerdActor);
                }
            }
        }
    }

    // Clean up our main list by removing all the HerdActors that are now in herds
    if (HerdedHerdActors.Num() > 0)
    {
        AvailableActors.RemoveAll([&HerdedHerdActors](const TWeakObjectPtr<AActor>& HerdActor)
        {
            return !HerdActor.IsValid() || HerdedHerdActors.Contains(HerdActor.Get());
        });
    }
}