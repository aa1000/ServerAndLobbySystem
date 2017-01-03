// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RnDTarget : TargetRules
{
	public RnDTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
        // Type = TargetType.Server;
       // UEBuildConfiguration.bCompileSteamOSS = true;
        bUsesSteam = true;
    }

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.Add("RnD");
	}
}
