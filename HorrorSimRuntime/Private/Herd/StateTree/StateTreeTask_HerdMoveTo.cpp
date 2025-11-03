#include "Herd/StateTree/StateTreeTask_HerdMoveTo.h"

#include "StateTreeExecutionContext.h"
#include "Herd/HerdManager.h"


FStateTreeTask_HerdMoveTo::FStateTreeTask_HerdMoveTo()
{
	bShouldCallTick = true;
}

AHerdManager* FStateTreeTask_HerdMoveTo::GetHerdManager(FStateTreeExecutionContext& Context) const
{
	UObject* Owner = Context.GetOwner();
	return Cast<AHerdManager>(Owner);
}

EStateTreeRunStatus FStateTreeTask_HerdMoveTo::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AHerdManager* HerdManager = GetHerdManager(Context);
	if (!HerdManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("StateTreeTask_OrderHerdToMove: EnterState failed, Owner is not an AHerdManager."));
		return EStateTreeRunStatus::Failed; // Fail the task
	}
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	HerdManager->OrderHerdToMove(InstanceData.TargetLocation);

	// We've given the command. Now we wait for Tick to see when it's done.
	return EStateTreeRunStatus::Running;
}

void FStateTreeTask_HerdMoveTo::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	AHerdManager* HerdManager = GetHerdManager(Context);
	if (HerdManager)
		{
			HerdManager->StopHerdMovement();
		}
}

EStateTreeRunStatus FStateTreeTask_HerdMoveTo::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	AHerdManager* HerdManager = GetHerdManager(Context);
	if (!HerdManager)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	if (HerdManager->IsHerdMoveComplete())
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

#if WITH_EDITOR
FText FStateTreeTask_HerdMoveTo::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup,
	EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	FText Desc = FText::FromString("Herd Move To");
	return Desc;
}
#endif
