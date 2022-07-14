// Copyright 2021 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
using System.IO;
using UnrealBuildTool;

public class ArcGISSamples : ModuleRules
{
    public ArcGISSamples(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/ArcGISSamplesPrivatePCH.h";

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "WebBrowser"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "ArcGISMapsSDK",
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Projects",
            "Slate",
            "SlateCore"
        });

        if (Target.Platform == UnrealTargetPlatform.Android || Target.Platform == UnrealTargetPlatform.IOS)
        {
            PublicDependencyModuleNames.AddRange(new string[]
            {
                "WebAuth"
            });
        }
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "ArcGISForUnrealHost_APL.xml"));

        }
    }
}
