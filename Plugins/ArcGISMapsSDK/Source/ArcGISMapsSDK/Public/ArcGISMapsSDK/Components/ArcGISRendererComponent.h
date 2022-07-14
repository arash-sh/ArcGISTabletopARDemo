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

#include "Components/MeshComponent.h"
#include "CoreMinimal.h"

#include "ArcGISMapsSDK/Components/ArcGISSceneComponent.h"

#include "ArcGISRendererComponent.generated.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
class FArcGISRenderer;
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri

UCLASS(ClassGroup = (ArcGISMapsSDK), meta = (BlueprintSpawnableComponent), Category = "ArcGISMapsSDK|ArcGISRendererComponent")
class ARCGISMAPSSDK_API UArcGISRendererComponent : public UArcGISSceneComponent
{
	GENERATED_BODY()

public:
	UArcGISRendererComponent();

	uint32 GetLayerIdByMeshComponent(const UMeshComponent* InMeshComponent) const;

	// UActorComponent
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// IArcGISMapsSDKSubsystemListener
	virtual void OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent) override;

protected:
	TSharedPtr<Esri::ArcGISMapsSDK::Renderer::FArcGISRenderer> ArcGISRenderer;

private:
	bool bTerrainOcclusionEnabled = true;

	UFUNCTION()
	void OnEditorModeChanged();

	UFUNCTION()
	void OnMeshCollidersEnabledChanged();
};
