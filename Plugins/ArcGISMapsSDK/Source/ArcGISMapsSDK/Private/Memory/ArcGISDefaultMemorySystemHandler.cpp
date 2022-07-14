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
#include "ArcGISMapsSDK/Memory/ArcGISDefaultMemorySystemHandler.h"

#include "DynamicRHI.h"

FArcGISDefaultMemorySystemHandler::~FArcGISDefaultMemorySystemHandler() = default;

FArcGISMemoryQuotas FArcGISDefaultMemorySystemHandler::GetMemoryQuotas()
{
	FTextureMemoryStats textureMemStats;
	RHIGetTextureMemoryStats(textureMemStats);

	auto platformMemoryStats = FPlatformMemory::GetStats();

#if PLATFORM_ANDROID || PLATFORM_HOLOLENS || PLATFORM_IOS
	return FArcGISMemoryQuotas{static_cast<int64>(FMath::Max(500.0f, platformMemoryStats.TotalPhysical * 0.4f / 1024.0f / 1024.0f)), -1};
#else
	return FArcGISMemoryQuotas{static_cast<int64>(FMath::Max(500.0f, platformMemoryStats.TotalPhysical * 0.8f / 1024.0f / 1024.0f)),
							   static_cast<int64>(FMath::Max(500.0f, textureMemStats.DedicatedVideoMemory * 0.8f / 1024.0f / 1024.0f))};
#endif
}

void FArcGISDefaultMemorySystemHandler::Initialize(IArcGISMemorySystem* InMemorySystem)
{
	MemorySystem = InMemorySystem;
}
