// COPYRIGHT 1995-2021 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Environmental Systems Research Institute, Inc.
// Attn: Contracts and Legal Services Department
// 380 New York Street
// Redlands, California, 92373
// USA
//
// email: contracts@esri.com

#if PLATFORM_MAC
#include "ArcGISMapsSDK/CAPI/Platform.h" // IWYU pragma: associated

#include "Interfaces/IPluginManager.h"

void* GetRTCDLLHandle()
{
	static void* dllHandle = nullptr;

	if (!dllHandle)
	{
		// Get the base directory of this plugin
		FString baseDir = IPluginManager::Get().FindPlugin("ArcGISMapsSDK")->GetBaseDir();

		// Add on the relative location of the third party dll and load it
		FString Path = FPaths::Combine(*baseDir, TEXT("Binaries/ThirdParty/ArcGISMapsSDK/Mac"));

		FPlatformProcess::PushDllDirectory(*Path);
		dllHandle = FPlatformProcess::GetDllHandle(*(Path + "/libruntimecore.dylib"));
		FPlatformProcess::PopDllDirectory(*Path);
	}

	return dllHandle;
}
#endif
