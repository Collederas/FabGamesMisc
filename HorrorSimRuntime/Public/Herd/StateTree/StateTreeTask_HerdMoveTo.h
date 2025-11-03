// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "Herd/HerdManager.h"
#include "StateTreeTask_HerdMoveTo.generated.h"

USTRUCT(BlueprintType)
struct HORRORSIMRUNTIME_API FHerdMoveToTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> UserActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	FVector TargetLocation = FVector::Zero();
};


USTRUCT(meta = (DisplayName = "Herd Move To", Category="Herd"))
struct HORRORSIMRUNTIME_API FStateTreeTask_HerdMoveTo : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FHerdMoveToTaskInstanceData;
	
	FStateTreeTask_HerdMoveTo();
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	AHerdManager* GetHerdManager(FStateTreeExecutionContext& Context) const;
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
	virtual FName GetIconName() const override
	{
		return FName("StateTreeEditorStyle|Node.Tag");
	}
	virtual FColor GetIconColor() const override
	{
		return UE::StateTree::Colors::Blue;
	}
#endif
};