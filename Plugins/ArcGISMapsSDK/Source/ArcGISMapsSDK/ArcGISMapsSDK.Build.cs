// COPYRIGHT 1995-2021 ESRI
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
using System;
using System.IO;
using UnrealBuildTool;

public class ArcGISMapsSDK : ModuleRules
{
    public ArcGISMapsSDK(ReadOnlyTargetRules Target) : base(Target)
    {
        bEnableExceptions = true;
        bEnforceIWYU = false;

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Private/ArcGISMapsSDKPrivatePCH.h";

        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "Internal"));

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "Engine",
            "GeoReferencing"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "CoreUObject",
            "EngineSettings",
            "Projects",
            "RenderCore",
            "RHI"
        });

        if (Target.Type == TargetType.Editor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "AndroidLowMemoryHandlerUPL.xml"));

            // Copy library files to the building folder
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "ArcGISMapsSDK_APL.xml"));

            // Add the library
            PublicAdditionalLibraries.Add("$(PluginDir)/Binaries/ThirdParty/ArcGISMapsSDK/Android/arm64/libruntimecore.so");
        }

        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            // Add the import library
            PublicAdditionalFrameworks.Add(
                new Framework(
                    "Runtimecore",
                    Path.Combine(ModuleDirectory, "..", "..", "Binaries/ThirdParty/ArcGISMapsSDK/IOS/Runtimecore.framework"),
                    null,
                    true
                )
            );

            PublicFrameworks.AddRange(new string[] {
                "CoreText"
            });
        }

        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/ArcGISMapsSDK/Mac/libruntimecore.dylib");
        }

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.AddRange(new string[] {
                "_CRT_SECURE_NO_WARNINGS"
            });

            // Add the import library
            PublicAdditionalLibraries.Add("$(PluginDir)/Binaries/ThirdParty/ArcGISMapsSDK/Win64/runtimecore.lib");

            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/ArcGISMapsSDK/Win64/runtimecore.dll");

            PublicDelayLoadDLLs.Add("runtimecore.dll");
        }

        if (Target.Platform == UnrealTargetPlatform.HoloLens)
        {
            PublicDefinitions.AddRange(new string[] {
                "_CRT_SECURE_NO_WARNINGS"
            });

            if (Target.WindowsPlatform.Architecture == WindowsArchitecture.x64)
            {
                // Add the import library
                PublicAdditionalLibraries.Add("$(PluginDir)/Binaries/ThirdParty/ArcGISMapsSDK/HoloLens/x64/runtimecore.lib");

                RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/ArcGISMapsSDK/HoloLens/x64/runtimecore.dll");
            }
            else if (Target.WindowsPlatform.Architecture == WindowsArchitecture.ARM64)
            {
                // Add the import library
                PublicAdditionalLibraries.Add("$(PluginDir)/Binaries/ThirdParty/ArcGISMapsSDK/HoloLens/arm64/runtimecore.lib");

                RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/ArcGISMapsSDK/HoloLens/arm64/runtimecore.dll");
            }

            PublicDelayLoadDLLs.Add("runtimecore.dll");
        }
    }
}
