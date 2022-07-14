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
#pragma once

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Utils
{
struct ARCGISMAPSSDK_API Environment
{
public:
	static void Initialize(const FString& ProductName, const FString& ProductVersion, const FString& TempDirectory, const FString& InstallDirectory);
};
} // namespace Utils
} // namespace ArcGISMapsSDK
} // namespace Esri
