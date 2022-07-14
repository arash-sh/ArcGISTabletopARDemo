// COPYRIGHT 1995-2022 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Attn: Contracts and Legal Department
// Environmental Systems Research Institute, Inc.
// 380 New York Street
// Redlands, California 92373
// USA
//
// email: legal@esri.com
using System.IO;
using UnrealBuildTool;

public class ArcGISMapsSDKEditor : ModuleRules
{
    public ArcGISMapsSDKEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/ArcGISMapsSDKEditorPrivatePCH.h";

        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Internal"));

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "ApplicationCore",
            "ArcGISMapsSDK",
            "ArcGISSamples",
            "Core",
            "CoreUObject",
            "DesktopPlatform",
            "EditorFramework",
            "EditorInteractiveToolsFramework",
            "EditorStyle",
            "Engine",
            "InputCore",
            "InteractiveToolsFramework",
            "LevelEditor",
            "Projects",
            "PropertyEditor",
            "Slate",
            "SlateCore",
            "ToolMenus",
            "UnrealEd",
            "WebBrowser"
        });
    }
}
