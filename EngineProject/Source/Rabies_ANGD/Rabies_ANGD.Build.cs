// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Rabies_ANGD : ModuleRules
{
	public Rabies_ANGD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "Slate", "SlateCore", "UMG", "AIModule", "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemEOS", "CinematicCamera", "LevelSequence", "MovieScene", "NavigationSystem", "Niagara", "NiagaraCore", "AdvancedWidgets", "Media", "MediaAssets", "HTTP"});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
