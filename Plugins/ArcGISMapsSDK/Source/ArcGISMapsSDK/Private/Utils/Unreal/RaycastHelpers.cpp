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

#include "ArcGISMapsSDK/Utils/Unreal/RaycastHelpers.h"

int32 FRaycastHelpers::GetFeatureIndexByTriangleIndex(const UMeshComponent* InMeshComponent, uint32 InPrimitiveIndex)
{
	return InMeshComponent ? Cast<UArcGISMeshComponent>(InMeshComponent)->GetFeatureIndexByTriangleIndex(InPrimitiveIndex) : -1;
}
