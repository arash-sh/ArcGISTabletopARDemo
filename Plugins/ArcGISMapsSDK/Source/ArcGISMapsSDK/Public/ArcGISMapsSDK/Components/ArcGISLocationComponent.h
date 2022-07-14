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

#include "CoreMinimal.h"

#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISPoint.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISPoint.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Location/ArcGISRotation.h"
#include "ArcGISMapsSDK/Components/ArcGISSceneComponent.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoPosition.h"

#include "ArcGISLocationComponent.generated.h"

USTRUCT()
struct FArcGISRotationInternal
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ArcGISMapsSDK|ArcGISLocationComponent")
	float Heading = 0;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapsSDK|ArcGISLocationComponent")
	float Pitch = 0;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapsSDK|ArcGISLocationComponent")
	float Roll = 0;
};

UCLASS(ClassGroup = (ArcGISMapsSDK), meta = (BlueprintSpawnableComponent), Category = "ArcGISMapsSDK|ArcGISLocationComponent")
class ARCGISMAPSSDK_API UArcGISLocationComponent : public UArcGISSceneComponent
{
	GENERATED_BODY()

public:
	UArcGISLocationComponent();

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISLocationComponent")
	UArcGISPoint* GetPosition();
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISLocationComponent")
	void SetPosition(UArcGISPoint* InPosition);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISLocationComponent")
	UArcGISRotation* GetRotation();
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISLocationComponent")
	void SetRotation(UArcGISRotation* InRotation);

	// UObject
#if WITH_EDITORONLY_DATA
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// UActorComponent
	void OnUnregister() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// USceneComponent
	void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport = ETeleportType::None) override;

	// IArcGISMapsSDKSubsystemListener
	void OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent) override;

protected:
	UPROPERTY(EditAnywhere, Category = "ArcGISLocationComponent")
	FGeoPosition Position;

	UPROPERTY(EditAnywhere, Category = "ArcGISLocationComponent")
	FArcGISRotationInternal Rotation;

	// UActorComponent
	virtual void BeginPlay() override;
	virtual void OnComponentCreated() override;

private:
	UFUNCTION()
	void UpdateTransform();

	void UpdateGeoPositionAndRotation();

	bool bInternalHasChanged{false};
	bool bNeedsInitialization{false};
	bool bTransformHasChanged{false};
};
