
#include "LyraWheeledVehicle.h"

#include "AbilitySystemComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "HorrorSimGameplayTags.h"
#include "HorrorSimLogChannels.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/FuelAttributeSet.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "Camera/LyraCameraComponent.h"
#include "Camera/VehicleCameraMode_FirstPerson.h"
#include "Character/LyraCharacter.h"
#include "Character/LyraHealthComponent.h"
#include "Character/LyraPawnData.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/LyraInputComponent.h"
#include "Player/LyraPlayerState.h"


class ALyraPlayerState;
class ULyraAbilitySet;

ALyraWheeledVehicle::ALyraWheeledVehicle(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<ULyraAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<ULyraHealthSet>(TEXT("HealthSet"));
	FuelAttributeSet = CreateDefaultSubobject<UFuelAttributeSet>(TEXT("VehicleAttributeSet"));

	FuelAttributeSet->OnOutOfFuel.AddUObject(this, &ThisClass::OnOutOfFuel);

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	CameraComponent = CreateDefaultSubobject<ULyraCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 500.0f));
	
	HealthComponent = CreateDefaultSubobject<ULyraHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
}


void ALyraWheeledVehicle::BeginPlay()
{
	Super::BeginPlay();
	
	ULyraAbilitySystemComponent* LyraASC = GetLyraAbilitySystemComponent();
	check(LyraASC);

	HealthComponent->InitializeWithAbilitySystem(LyraASC);

	if (StartingAbilitySet)
	{
		StartingAbilitySet->GiveToAbilitySystem(LyraASC, nullptr);
	}
}

void ALyraWheeledVehicle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	HealthComponent->UninitializeFromAbilitySystem();
	Super::EndPlay(EndPlayReason);
}

void ALyraWheeledVehicle::OnDeathStarted(AActor* OwningActor)
{
	UE_LOG(LogHorrorSim, Log, TEXT("OnDeathStarted"));
}

void ALyraWheeledVehicle::OnDeathFinished(AActor* OwningActor)
{
	if (bDestroyOnDeath)
		SetLifeSpan(1.0);
}

void ALyraWheeledVehicle::OnOutOfFuel(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	UE_LOG(LogHorrorSim, Log, TEXT("OnOutOfFuel"));
	UChaosVehicleMovementComponent* VehicleMovement = GetVehicleMovement();
	VehicleMovement->SetThrottleInput(0.f);
	VehicleMovement->SetBrakeInput(1.f);
}

void ALyraWheeledVehicle::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!Driver)
		return;
	
	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Driver))
	{
		if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
		{
			LyraASC->AbilityInputTagPressed(InputTag);
		}
	}
}

void ALyraWheeledVehicle::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!Driver)
		return;
	
	if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Driver))
	{
		if (ULyraAbilitySystemComponent* LyraASC = PawnExtComp->GetLyraAbilitySystemComponent())
		{
			LyraASC->AbilityInputTagReleased(InputTag);
		}
	}
}


void ALyraWheeledVehicle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ALyraWheeledVehicle::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController != nullptr)
	{
		if (ALyraPlayerState* LyraPS = NewController->GetPlayerState<ALyraPlayerState>())
		{
			ULyraAbilitySystemComponent* LyraASC = LyraPS->GetLyraAbilitySystemComponent();
			// LyraASC->InitAbilityActorInfo(LyraPS, this);
			
			for (ULyraAbilitySet* LyraAbilitySet : LyraPawnData->AbilitySets)
			{
				LyraAbilitySet->GiveToAbilitySystem(LyraASC, &GrantedHandles);
			}

			if (ULyraInputComponent* LyraInputComponent = Cast<ULyraInputComponent>(NewController->InputComponent))
			{
				TArray<uint32> BindHandles;
				LyraInputComponent->BindAbilityActions(LyraPawnData->InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
				LyraInputComponent->BindNativeAction(LyraPawnData->InputConfig, HorrorSimGameplayTags::InputTag_Vehicle_QuitDriving, ETriggerEvent::Triggered, this, &ThisClass::EjectDriver, /*bLogIfNotFound=*/ false);
			}
		}

		CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ALyraWheeledVehicle::DetermineCameraMode);
	}
}

void ALyraWheeledVehicle::UnPossessed()
{
	AController* OldController = GetController();

	if (OldController != nullptr)
	{
		if (ALyraPlayerState* LyraPS = OldController->GetPlayerState<ALyraPlayerState>())
		{
			ULyraAbilitySystemComponent* LyraASC = LyraPS->GetLyraAbilitySystemComponent();
			GrantedHandles.TakeFromAbilitySystem(LyraASC);
		}
		CameraComponent->DetermineCameraModeDelegate.Unbind();
	}
	Super::UnPossessed();
}

FGenericTeamId ALyraWheeledVehicle::GetGenericTeamId() const
{
	return MyTeamID;
}

ULyraAbilitySystemComponent* ALyraWheeledVehicle::GetLyraAbilitySystemComponent() const
{
	return Cast<ULyraAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ALyraWheeledVehicle::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALyraWheeledVehicle::SetDriver(ALyraCharacter* InDriver)
{
	Driver = InDriver;
}

ALyraCharacter* ALyraWheeledVehicle::GetDriver() const
{
	return Driver;
}

void ALyraWheeledVehicle::EjectDriver()
{
	if (!Driver)
		return;
	
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (!PC)
		return;
	
	Driver->SetActorHiddenInGame(false);
	Driver->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	FTransform VehicleRootTransform = GetMesh()->GetSocketTransform(FName("RootSocket"));
	
	if (HasAuthority())
	{
		PC->Possess(Driver);
	}
	Driver->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Driver->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Driver->SetActorTransform(VehicleRootTransform);
	Driver->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PC->SetControlRotation(VehicleRootTransform.GetRotation().Rotator());

	if (ULyraAbilitySystemComponent* DriverASC = Driver->GetLyraAbilitySystemComponent())
	{
		const FGameplayTagContainer WithTags = FGameplayTagContainer(HorrorSimGameplayTags::Ability_Type_Action_Drive);
		const FGameplayTagContainer WithoutTags = FGameplayTagContainer();
		DriverASC->CancelAbilities(&WithTags, &WithoutTags);
	}
	
	SetDriver(nullptr);
}

void ALyraWheeledVehicle::SwitchCameraMode()
{
	if (CurrentCameraMode == ELyraVehicleCameraMode::ThirdPerson)
	{
		CurrentCameraMode = ELyraVehicleCameraMode::FirstPerson;
	}
	else
	{
		CurrentCameraMode = ELyraVehicleCameraMode::ThirdPerson;
	}
}

float ALyraWheeledVehicle::GetMaxFuel()
{
	return (FuelAttributeSet ? FuelAttributeSet->GetMaxFuel() : 0.0f);
}

TSubclassOf<ULyraCameraMode> ALyraWheeledVehicle::DetermineCameraMode()
{
	switch (CurrentCameraMode)
	{
	case ELyraVehicleCameraMode::FirstPerson:
		return FirstPersonModeClass;
	case ELyraVehicleCameraMode::ThirdPerson:
		return ThirdPersonModeClass;
	default:
		return ThirdPersonModeClass;
	}
}
