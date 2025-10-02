#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "WheeledVehiclePawn.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "Camera/LyraCameraMode.h"
#include "Teams/LyraTeamAgentInterface.h"

#include "LyraWheeledVehicle.generated.h"

class ULyraCameraComponent;
class ULyraHealthComponent;
class ALyraCharacter;
struct FLyraAbilitySet_GrantedHandles;
class ULyraAbilitySystemComponent;
class ULyraPawnData;
class ULyraPawnExtensionComponent;

UENUM(BlueprintType)
enum class ELyraVehicleCameraMode : uint8
{
	ThirdPerson	UMETA(DisplayName = "Third Person"),
	FirstPerson	UMETA(DisplayName = "First Person")
};

UCLASS()
class HORRORSIMRUNTIME_API ALyraWheeledVehicle : public AWheeledVehiclePawn, public IAbilitySystemInterface, public ILyraTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALyraWheeledVehicle(const FObjectInitializer& ObjectInitializer);
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Vehicle")
	ULyraAbilitySystemComponent* GetLyraAbilitySystemComponent() const;	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Vehicle")
	void SetDriver(ALyraCharacter* InDriver);

	UFUNCTION(BlueprintCallable, Category = "Lyra|Vehicle")
	ALyraCharacter* GetDriver() const;
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Vehicle")
	void EjectDriver();

	UFUNCTION(BlueprintCallable, Category = "Lyra|Vehicle")
	void SwitchCameraMode();
	
protected:
	TSubclassOf<ULyraCameraMode> DetermineCameraMode();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const ULyraPawnData> LyraPawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const ULyraAbilitySet> StartingAbilitySet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDestroyOnDeath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	ELyraVehicleCameraMode CurrentCameraMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ULyraCameraMode> FirstPersonModeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ULyraCameraMode> ThirdPersonModeClass;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Lyra|Abilities")
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const class ULyraHealthSet> HealthSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraCameraComponent> CameraComponent;

	UPROPERTY()
	TObjectPtr<class ALyraCharacter> Driver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Vehicle", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULyraHealthComponent> HealthComponent;

	UPROPERTY(EditAnywhere, Category = "Team")
	FGenericTeamId MyTeamID;
	
	FLyraAbilitySet_GrantedHandles GrantedHandles;
};

