#include "HorrorSimGameplayTags.h"

#include "HorrorSimLogChannels.h"
#include "Engine/EngineTypes.h"


namespace HorrorSimGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Vehicle_QuitDriving, "InputTag.Vehicle.QuitDriving", "Eject Driver from Lyra Vehicle in HorrorSim plugin.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Type_Action_Drive, "Ability.Type.Action.Drive", "Driving a vehicle");

	
	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogHorrorSim, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}

