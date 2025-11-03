// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HorrorSimRuntime : ModuleRules
{
	public HorrorSimRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"FontanaGame", 
				"StateTreeModule",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"ChaosVehicles",
				"GameplayAbilities",
				"GameplayTags",
				"AIModule",
				"EnhancedInput",
				"UMG",
				"CommonUI",
				"StateTreeModule", 
				"GameplayStateTreeModule",
				"DeveloperSettings"
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
