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

#include "Kismet/BlueprintFunctionLibrary.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

#include "AtmosphereHelpers.generated.h"

UCLASS()
class UAtmosphereHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "ArcGISMapsSDK")
	static FVector GetGlobalViewSkyAtmospherePositionFrom(UArcGISMapComponent* InMapComponent, const FVector& Position);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "ArcGISMapsSDK")
	static FVector GetLocalViewSkyAtmospherePositionFrom(const FVector& Position);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "ArcGISMapsSDK")
	static float GetAerialPerspectiveViewDistanceScaleFrom(UArcGISMapComponent* InMapComponent,
														   const FVector& Position,
														   float AtmosphereHeight,
														   float AtmosphereScale,
														   float BlendingWidth);
};
