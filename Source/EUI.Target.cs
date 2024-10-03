// Copyright (C) 2024 Cindy Chen. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class EUITarget : TargetRules
{
	public EUITarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "EUI" } );
	}
}
