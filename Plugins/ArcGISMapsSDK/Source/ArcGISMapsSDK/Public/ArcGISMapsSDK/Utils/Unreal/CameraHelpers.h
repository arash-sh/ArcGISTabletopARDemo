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

#include "Kismet/BlueprintFunctionLibrary.h"

#include "ArcGISMapsSDK/Actors/ArcGISMapActor.h"

#include "CameraHelpers.generated.h"

UCLASS()
class UCameraHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "ArcGISMapsSDK")
	static FVector GetActiveCameraPosition(const UObject* InWorldContextObject);

	static FMatrix GetActiveCameraViewProjMatrix(const UObject* InWorldContextObject);
};
