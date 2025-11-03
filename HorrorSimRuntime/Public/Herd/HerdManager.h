
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HerdManager.generated.h"

class AAIController;
class UStateTreeComponent;

UCLASS()
class HORRORSIMRUNTIME_API AHerdManager : public AActor
{
	GENERATED_BODY()

public:
	AHerdManager();

	/** This is the "hand-off" function called by the Subsystem right after spawning */
	void InitializeHerd(const TArray<AActor*>& HerdMembers);

	/**
	 * Commands all members of the herd to move to a specific location.
	 * @param TargetLocation The world-space location to move to.
	 */
	UFUNCTION(BlueprintCallable, Category = "Herd Manager")
	void OrderHerdToMove(FVector TargetLocation);

	/**
	 * Commands all members of the herd to stop all current movement.
	 */
	UFUNCTION(BlueprintCallable, Category = "Herd Manager")
	void StopHerdMovement();

	/**
	 * Checks if all members of the herd have completed their move command.
	 * @return True if all members are idle or at their destination, false if any are still moving.
	 */
	UFUNCTION(BlueprintPure, Category = "Herd Manager")
	bool IsHerdMoveComplete() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UStateTreeComponent* StateTreeComponent;

	// List of controllers we've taken over
	UPROPERTY()
	TArray<TWeakObjectPtr<AAIController>> HerdMemberControllers;

	UPROPERTY()
	TMap<TWeakObjectPtr<AAIController>, float> OriginalActorSpeeds;
	
};
