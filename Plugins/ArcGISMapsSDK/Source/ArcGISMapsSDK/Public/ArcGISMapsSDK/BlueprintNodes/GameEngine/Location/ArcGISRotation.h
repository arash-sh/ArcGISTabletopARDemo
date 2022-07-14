// COPYRIGHT 1995-2021 ESRI
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

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "ArcGISRotation.generated.h"

namespace Esri
{
namespace GameEngine
{
namespace Location
{
class ArcGISRotation;
} // namespace Location
} // namespace GameEngine
} // namespace Esri

UCLASS(BlueprintType, Blueprintable)
class ARCGISMAPSSDK_API UArcGISRotation : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISRotation")
	static UArcGISRotation* CreateArcGISRotation(double pitch, double roll, double heading);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISRotation")
	double GetHeading();

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISRotation")
	double GetPitch();

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISRotation")
	double GetRoll();

	TSharedPtr<Esri::GameEngine::Location::ArcGISRotation> APIObject;
};
