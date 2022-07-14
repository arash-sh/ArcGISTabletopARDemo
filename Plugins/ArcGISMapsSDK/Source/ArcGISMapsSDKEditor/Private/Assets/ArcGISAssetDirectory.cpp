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
#include "Assets/ArcGISAssetDirectory.h"

#include "Assets/ArcGISAssetRepo.h"

FSoftObjectPath FArcGISAssetDirectory::Assets[] = {FString(TEXT("/ArcGISMapsSDK/Materials/Tile.Tile")),
												   FString(TEXT("/ArcGISMapsSDK/Materials/SceneNode.SceneNode"))};

void FArcGISAssetDirectory::LoadForCook()
{
	auto& arcGISAssetRepo = FArcGISAssetRepo::GetInstance();

	for (int32 i = 0; i < UE_ARRAY_COUNT(FArcGISAssetDirectory::Assets); ++i)
	{
		arcGISAssetRepo.LoadAndAdd(FArcGISAssetDirectory::Assets[i]);
	}
}

void FArcGISAssetDirectory::ReleaseAll()
{
	FArcGISAssetRepo::GetInstance().Empty();
}
