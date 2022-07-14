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

#include "CoreMinimal.h"

#include "UObject/GCObject.h"
#include "UObject/SoftObjectPtr.h"

class FArcGISAssetRepo : public FGCObject, public TArray<UObject*>
{
public:
	static FArcGISAssetRepo& GetInstance();

	UObject* LoadAndAdd(const FSoftObjectPath& AssetPath);

	// FGCObject
	void AddReferencedObjects(FReferenceCollector& Collector) override;
	FString GetReferencerName() const override;
};
