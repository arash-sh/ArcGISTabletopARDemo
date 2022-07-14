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

#include "Components/SkyAtmosphereComponent.h"
#include "CoreMinimal.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISSceneComponent.h"

#include "ArcGISSkyAtmosphereRepositionComponent.generated.h"

UCLASS(ClassGroup = (ArcGISMapsSDK), meta = (BlueprintSpawnableComponent), Category = "ArcGISMapsSDK|ArcGISSkyAtmosphereComponent")
class ARCGISMAPSSDK_API UArcGISSkyAtmosphereRepositionComponent : public UArcGISSceneComponent
{
	GENERATED_BODY()

public:
	UArcGISSkyAtmosphereRepositionComponent();

	// UActorComponent
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OnUnregister() override;

	// IArcGISMapsSDKSubsystemListener
	void OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent) override;

private:
	void Initialize();

	UFUNCTION()
	void UpdateSkyAtmospherePositionAndBlending();

	void UpdateSkyAtmospherePosition(const FVector3d& CameraPosition);
	void UpdateAtmosphereBlending(const FVector3d& CameraPosition);

	USkyAtmosphereComponent* SkyAtmosphereComponent{nullptr};

	FVector3d LastCameraPosition = FVector3d(0, 0, 0);
	bool bInitialized = false;
};
