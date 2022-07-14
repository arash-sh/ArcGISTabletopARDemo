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

#include "Components/SceneComponent.h"
#include "CoreMinimal.h"

#include "ArcGISMapsSDK/Subsystem/IArcGISMapsSDKSubsystemListener.h"

#include "ArcGISSceneComponent.generated.h"

UCLASS(Abstract)
class ARCGISMAPSSDK_API UArcGISSceneComponent : public USceneComponent, public IArcGISMapsSDKSubsystemListener
{
	GENERATED_BODY()

public:
	// UActorComponent
	void OnRegister() override;
	void OnUnregister() override;

	// IArcGISMapsSDKSubsystemListener
	void OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent) override;

#if WITH_EDITORONLY_DATA
	bool IsInEditorPreviewActor() const;
#endif

protected:
	TWeakObjectPtr<UArcGISMapComponent> MapComponent;
};
