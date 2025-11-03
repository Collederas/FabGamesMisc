// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/LyraActivatableWidget.h"
#include "HorrorSimInventoryContainerWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORRORSIMRUNTIME_API UHorrorSimInventoryContainerWidget : public ULyraActivatableWidget
{
	GENERATED_BODY()

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

private:
	/** Stores the original value of bTabNavigation so we can restore it. */
	bool bWasTabNavigationEnabled = true; // Default to true
};
