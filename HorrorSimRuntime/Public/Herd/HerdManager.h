#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HerdManager.generated.h"

class UStateTreeComponent;
class AAIController;
class USphereComponent;

USTRUCT(BlueprintType)
struct FHerdMemberData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Herd")
	TWeakObjectPtr<AActor> Member;

	UPROPERTY(VisibleAnywhere, Category = "Herd")
	FVector FormationOffset = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Category = "Herd")
	bool bOriginalCollisionState = true;

	UPROPERTY(VisibleAnywhere, Category = "Herd")
	TEnumAsByte<ECollisionChannel> OriginalCollisionChannel;

	UPROPERTY(VisibleAnywhere, Category = "Herd")
	TEnumAsByte<EMovementMode> OriginalMovementMode = EMovementMode::MOVE_None;
	
	UPROPERTY(VisibleAnywhere, Category = "Herd")
	float OriginalMaxWalkSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Herd")
	float ZOffset = 0.0f;
};

UCLASS()
class HORRORSIMRUNTIME_API AHerdManager : public ACharacter
{
	GENERATED_BODY()

public:
	AHerdManager(const FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable, Category = "Herd")
	void InitializeHerd(const TArray<AActor*>& Members);

	UFUNCTION(BlueprintCallable, Category = "Herd")
	void OrderHerdToMove(const FVector& GroupTargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Herd")
	void StopHerdMovement() const;

	UFUNCTION(BlueprintPure, Category = "Herd")
	bool IsHerdMoveComplete() const;
	
	UFUNCTION(BlueprintCallable, Category = "Herd")
	void DisbandHerd();

	UFUNCTION(BlueprintCallable, Category = "Herd")
	void SetHerdSpeed(float Speed);

protected:
	/** The StateTree component for managing AI behavior. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStateTreeComponent* StateTreeComponent;

	/** A dynamic sphere collider that encompasses the entire herd. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* HerdBoundsCollider;

	/** How quickly herd members interpolate to their target location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Herd|Movement")
	float MemberInterpolationSpeed = 6.0f;

	/** How quickly herd members interpolate to their target rotation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Herd|Movement")
	float MemberRotationInterpolationSpeed = 5.0f;

	/** How quickly the manager itself rotates to face its movement direction. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Herd|Movement")
	float ManagerRotationInterpolationSpeed = 3.0f;

	/** How far down to trace for ground when positioning members. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Herd|Movement")
	float GroundCheckTraceDistance = 500.0f;
	
	/** The maximum radius from the manager in which members will be placed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Herd|Movement")
	float HerdSpreadRadius = 300.0f;

	/** The acceptance radius for the AI's MoveToLocation requests. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Herd|AI")
	float PathPointArrivalRadius = 50.0f;

	UPROPERTY()
	TArray<FHerdMemberData> HerdMembersData;

	UFUNCTION(BlueprintPure, Category = "Herd")
	AAIController* GetHerdController() const;

private:
	void UpdateHerdMembers(float DeltaSeconds);
	void UpdateManagerRotation(float DeltaSeconds);
	
	/** Updates the position and size of the HerdBoundsCollider to encompass all members. */
	void UpdateHerdBounds();

	FCollisionQueryParams HerdTraceParams;
	FRotator TargetLookRotation;
};