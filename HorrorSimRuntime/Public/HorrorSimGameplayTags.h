// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace HorrorSimGameplayTags
{
	HORRORSIMRUNTIME_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);
	HORRORSIMRUNTIME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Vehicle_QuitDriving);
	HORRORSIMRUNTIME_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Action_Drive);

};
