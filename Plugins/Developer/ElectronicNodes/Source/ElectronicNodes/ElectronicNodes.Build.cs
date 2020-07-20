/* Copyright (C) 2019 Hugo ATTAL - All Rights Reserved
 * This plugin is downloadable from the UE4 Marketplace
 */

using System.IO;
using UnrealBuildTool;

public class ElectronicNodes : ModuleRules
{
	public ElectronicNodes(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		string fullEnginePath = Path.GetFullPath(Target.RelativeEnginePath);

		PublicIncludePaths.AddRange(
			new string[] { }
			);

		PrivateIncludePaths.AddRange(
			new string[] {
				fullEnginePath + "Source/Editor/AnimationBlueprintEditor/Private"
			}
			);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"UnrealEd",
				"GraphEditor",
				"MaterialEditor",
				"BlueprintGraph",
				"AnimGraph",
				"AnimationBlueprintEditor",
				"AudioEditor"
			}
			);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{}
			);
	}
}
