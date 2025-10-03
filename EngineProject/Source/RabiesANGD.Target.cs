// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RabiesANGDTarget : TargetRules
{
	public RabiesANGDTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V4;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
		ExtraModuleNames.Add("RabiesANGD");

		//GlobalDefinitions.Add("WITH_XAUDIO2=0");// disable legacy device at compile-time
    }
}
