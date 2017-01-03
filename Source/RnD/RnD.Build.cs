// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RnD : ModuleRules
{
	public RnD(TargetInfo Target)
	{
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bFasterWithoutUnity = true;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
        // Uncomment if you are using Slate UI
        //PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        //refresh your visual studio files from editor after adding this to avoid weird redline errors
        //linking steamworks
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Linux) || (Target.Platform == UnrealTargetPlatform.Mac))
        {
            PublicDependencyModuleNames.AddRange(new string[] { "Steamworks" });
        }
       // AddThirdPartyPrivateStaticDependencies(Target, "Steamworks");
       // PrivateDependencyModuleNames.Add("Steamworks");
        // Uncomment if you are using online features
        PrivateDependencyModuleNames.Add("OnlineSubsystem");
    }
}
