// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NoManIsAnIsland : ModuleRules
{
	public NoManIsAnIsland(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

		PublicDependencyModuleNames.AddRange(new string[] { "UMG", "Json", "JsonUtilities" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

    }
}
