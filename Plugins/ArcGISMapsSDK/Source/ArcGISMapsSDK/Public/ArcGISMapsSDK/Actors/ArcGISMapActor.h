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
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

#include "ArcGISMapActor.generated.h"

UCLASS(ComponentWrapperClass, hidecategories = (Actor, Cooking, Rendering, Collision, HLOD, Input, Replication))
class ARCGISMAPSSDK_API AArcGISMapActor : public AActor
{
	GENERATED_BODY()

public:
	AArcGISMapActor();

	TObjectPtr<UArcGISMapComponent> GetMapComponent() const;

private:
	UPROPERTY(Category = ArcGISMap, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArcGISMapComponent> MapComponent;
};
