// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RabiesANGD : ModuleRules
{
	public RabiesANGD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks", "Slate", "SlateCore", "UMG", "AIModule", "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemEOS", "CinematicCamera", "LevelSequence", "MovieScene", "NavigationSystem", "Niagara", "NiagaraCore", "AdvancedWidgets", "Media", "MediaAssets", "HTTP"});

        // Force-disable legacy XAudio2.7
        //PublicDefinitions.Add("XAUDIO_SUPPORTS_XAUDIO27=0");
        //PublicDefinitions.Add("WITH_XAUDIO2=0");

        PublicDefinitions.Add("XAUDIO_SUPPORTS_XAUDIO27=0");  // disable legacy
        PublicDefinitions.Add("WITH_XAUDIO2=1");               // keep XAudio2 backend alive
        PublicDefinitions.Add("XAUDIO_SUPPORTS_XAUDIO29=1");   // force 2.9

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
