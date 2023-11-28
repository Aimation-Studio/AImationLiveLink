// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class nlohmann_json : ModuleRules
{
	public nlohmann_json(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
        bEnableExceptions = true;
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		//PublicSystemIncludePaths.Add("$(ModuleDir)");

		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory // Updated path
				// ... add other public include paths required here ...
			}
		);
	}
}

