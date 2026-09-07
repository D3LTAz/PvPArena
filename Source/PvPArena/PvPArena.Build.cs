// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PvPArena : ModuleRules
{
	public PvPArena(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"PvPArena",
			"PvPArena/Variant_Platforming",
			"PvPArena/Variant_Platforming/Animation",
			"PvPArena/Variant_Combat",
			"PvPArena/Variant_Combat/AI",
			"PvPArena/Variant_Combat/Animation",
			"PvPArena/Variant_Combat/Gameplay",
			"PvPArena/Variant_Combat/Interfaces",
			"PvPArena/Variant_Combat/UI",
			"PvPArena/Variant_SideScrolling",
			"PvPArena/Variant_SideScrolling/AI",
			"PvPArena/Variant_SideScrolling/Gameplay",
			"PvPArena/Variant_SideScrolling/Interfaces",
			"PvPArena/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
