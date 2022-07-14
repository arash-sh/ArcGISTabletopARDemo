// COPYRIGHT 1995-2022 ESRI
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
#pragma warning(disable:4828)

#include "ArcGISMapsSDK/Utils/Environment.h"

#include "../buildnum.h"

#include "ArcGISMapsSDK/API/GameEngine/ArcGISRuntimeEnvironmentInternal.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISRuntimeClient.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISException.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Utils
{
void Environment::Initialize(const FString& ProductName, const FString& ProductVersion, const FString& TempDirectory, const FString& InstallDirectory)
{
	try
	{
		int32_t BuildNumParts[]{GE_BUILD_VER};
		char Version[255];
		snprintf(Version, 255, "%d.%d.%d", BuildNumParts[0], BuildNumParts[1], BuildNumParts[2]);

		GameEngine::ArcGISRuntimeEnvironmentInternal::SetRuntimeClient(Standard::ArcGISRuntimeClient::Unreal, Version);
		GameEngine::ArcGISRuntimeEnvironmentInternal::SetProductInfo(ProductName, ProductVersion);
		GameEngine::ArcGISRuntimeEnvironmentInternal::EnableBreakOnException(false);
		GameEngine::ArcGISRuntimeEnvironmentInternal::EnableLeakDetection(false);
		GameEngine::ArcGISRuntimeEnvironmentInternal::SetTempDirectory(TempDirectory);
		GameEngine::ArcGISRuntimeEnvironmentInternal::SetInstallDirectory(InstallDirectory);
	}
	catch (const Unreal::ArcGISException& Ex)
	{
		UE_LOG(LogTemp, Warning, TEXT("Environment::Initialize: %s"), *Ex.GetMessage());
	}
}
} // namespace Utils
} // namespace ArcGISMapsSDK
} // namespace Esri
