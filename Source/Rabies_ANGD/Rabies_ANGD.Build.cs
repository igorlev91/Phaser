// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Rabies_ANGD : ModuleRules
{
	public Rabies_ANGD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "CommonUI", "Core", "CoreUObject", "Engine", "InputCore" ,
          "DeveloperSettings",   "EnhancedInput", "OnlineSubsystem"});

        PrivateDependencyModuleNames.AddRange(new string[] { "CommonUI", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "Slate", "SlateCore", "UMG", "AIModule", "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemEOS", "CinematicCamera", "LevelSequence", "MovieScene", "NavigationSystem"  , "Niagara", "NiagaraCore", "AdvancedWidgets" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ReplicationGraph" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
