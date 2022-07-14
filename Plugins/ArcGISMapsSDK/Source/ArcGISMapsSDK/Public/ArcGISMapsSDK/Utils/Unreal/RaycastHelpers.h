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
#pragma once

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayer.h"
#include "ArcGISMapsSDK/Components/ArcGISMeshComponent.h"

#include "RaycastHelpers.generated.h"


USTRUCT(BlueprintType, Category = "ArcGISMapsSDK|ArcGISRayCastHit")
struct ARCGISMAPSSDK_API FArcGISRaycastHit
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "ArcGISMapsSDK|ArcGISRayCastHit")
	int32 FeatureId;
	UPROPERTY(BlueprintReadWrite, Category = "ArcGISMapsSDK|ArcGISRayCastHit")
	UArcGISLayer* Layer;
};

class FRaycastHelpers
{
public:
	static int32 GetFeatureIndexByTriangleIndex(const UMeshComponent* InMeshComponent, uint32 InPrimitiveIndex);
};
