// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class LunaraTEOM : ModuleRules
{
	public LunaraTEOM(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "Slate", "SlateCore", "SVGImporter" });

		PrivateDependencyModuleNames.AddRange(new string[] {"Slate","SlateCore","UMG",});

		var SvgImporterPath = Path.Combine(EngineDirectory, "Plugins", "Experimental", "SVGImporter");
		if (Directory.Exists(SvgImporterPath))
		{
			PrivateDependencyModuleNames.Add("SVGImporter");
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
