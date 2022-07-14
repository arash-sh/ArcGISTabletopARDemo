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
#include "ArcGISMapsSDKModule.h"

#include "GeneralProjectSettings.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/CoreDelegates.h"
#if PLATFORM_WINDOWS
#include "Interfaces/IPluginManager.h"
#include "Misc/MessageDialog.h"
#endif // PLATFORM_WINDOWS
#include "ShaderCore.h"

#include "ArcGISMapsSDK/Utils/Environment.h"

void FArcGISMapsSDKModule::OnPostEngineInit()
{
#if PLATFORM_ANDROID
	extern FString GInternalFilePath;

	const auto& tempDirectory = GInternalFilePath + "/../cache";
#elif PLATFORM_IOS
	const auto& tempDirectory = FString([NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0]) + TEXT("/");
#else
	const auto& tempDirectory = FPlatformProcess::UserTempDir();
#endif

#if WITH_EDITOR
	const auto& installDirectory = FPaths::ProjectDir();
#else
	const auto& installDirectory = FPlatformProcess::GetCurrentWorkingDirectory();
#endif // WITH_EDITOR

	const UGeneralProjectSettings& ProjectSettings = *GetDefault<UGeneralProjectSettings>();

	const auto& productName = ProjectSettings.ProjectName;
	const auto& productVersion = ProjectSettings.ProjectVersion;

	Esri::ArcGISMapsSDK::Utils::Environment::Initialize(productName, productVersion, tempDirectory, installDirectory);
}

void FArcGISMapsSDKModule::ShutdownModule()
{
	ResetAllShaderSourceDirectoryMappings();

#if PLATFORM_WINDOWS
	if (RuntimeCoreHandle)
	{
		FPlatformProcess::FreeDllHandle(RuntimeCoreHandle);
		RuntimeCoreHandle = nullptr;
	}
#endif // PLATFORM_WINDOWS
}

#define LOCTEXT_NAMESPACE "FArcGISMapsSDKModule"

void FArcGISMapsSDKModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ArcGISMapsSDK"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/ArcGISMapsSDK"), PluginShaderDir);

#if PLATFORM_WINDOWS
	// Get the base directory of this plugin
	FString baseDir = IPluginManager::Get().FindPlugin("ArcGISMapsSDK")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString thirdPartyPath = FPaths::Combine(*baseDir, TEXT("Binaries/ThirdParty/ArcGISMapsSDK"));

	FString runtimecorePath = FPaths::Combine(*thirdPartyPath, TEXT("Win64/runtimecore.dll"));

	RuntimeCoreHandle = !runtimecorePath.IsEmpty() ? FPlatformProcess::GetDllHandle(*runtimecorePath) : nullptr;

	if (!RuntimeCoreHandle)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ArcGIS Maps SDK for Unreal Engine", "ArcGIS Maps SDK for Unreal Engine failed to load"));

		return;
	}
#endif // PLATFORM_WINDOWS

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FArcGISMapsSDKModule::OnPostEngineInit);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FArcGISMapsSDKModule, ArcGISMapsSDK)
