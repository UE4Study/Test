// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StrategyGame : ModuleRules
{
	public StrategyGame(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
                "AIModule",
				"GameplayTasks",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"StrategyGameLoadingScreen"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Slate",
				"SlateCore",
			}
		);


		PrivateIncludePaths.AddRange(
			new string[] {
				"StrategyGame/Private/UI/Menu",
				"StrategyGame/Private/UI/Style",
				"StrategyGame/Private/UI/Widgets",
			}
		);
	}
}
