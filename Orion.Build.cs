// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Orion : ModuleRules
{
	public Orion(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "Core", "CoreUObject", "Engine", "InputCore" });
	}
}
