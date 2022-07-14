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

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Subsystem/IArcGISMapsSDKSubsystemListener.h"

#include "ArcGISActor.generated.h"

UCLASS(Abstract)
class ARCGISMAPSSDK_API AArcGISActor : public AActor, public IArcGISMapsSDKSubsystemListener
{
	GENERATED_BODY()

public:
	// AActor
	void PostRegisterAllComponents() override;
	void UnregisterAllComponents(bool bForReregister = false) override;

	// IArcGISMapsSDKSubsystemListener
	void OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent) override;

protected:
	UArcGISMapComponent* MapComponent{nullptr};
};
