// Copyright (C) 2024 Cindy Chen. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class EUIEditorTarget : TargetRules
{
	public EUIEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "EUI" } );
	}
}
