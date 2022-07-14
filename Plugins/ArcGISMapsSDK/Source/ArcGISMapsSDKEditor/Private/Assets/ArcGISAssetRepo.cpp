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
#include "Assets/ArcGISAssetRepo.h"

FArcGISAssetRepo& FArcGISAssetRepo::GetInstance()
{
	static FArcGISAssetRepo arcGISAssetRepo;
	return arcGISAssetRepo;
}

UObject* FArcGISAssetRepo::LoadAndAdd(const FSoftObjectPath& AssetPath)
{
	auto asset = AssetPath.TryLoad();

	if (asset)
	{
		AddUnique(asset);
	}

	return asset;
}

void FArcGISAssetRepo::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObjects(*this);
}

FString FArcGISAssetRepo::GetReferencerName() const
{
	return TEXT("FArcGISAssetRepo");
}
