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
#include "InteractiveToolBuilder.h"
#include "UObject/NoExportTypes.h"

#include "ArcGISMapsSDK/Utils/GeoCoord/GeoPosition.h"
#include "ArcGISMapsSDK/Components/ArcGISCameraComponent.h"

#include "ArcGISMapCreatorCameraTool.generated.h"

USTRUCT()
struct FModesUICameraRotator
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = CameraRotation, meta = (ClampMin = "0", ClampMax = "360", DisplayName = "Heading"))
	float CameraHeading;

	UPROPERTY(EditAnywhere, Category = CameraRotation, meta = (ClampMin = "0", ClampMax = "360", DisplayName = "Pitch"))
	float CameraPitch;

	UPROPERTY(EditAnywhere, Category = CameraRotation, meta = (ClampMin = "0", ClampMax = "360", DisplayName = "Roll"))
	float CameraRoll;
};

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorCameraToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
	virtual void PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const;
};

UCLASS(Transient)
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorCameraToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Camera)
	FGeoPosition CameraPosition;

	UPROPERTY(EditAnywhere, Category = Camera)
	FModesUICameraRotator CameraRotation;
};

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorCameraTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	virtual void Setup() override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

	UPROPERTY()
	TObjectPtr<UArcGISMapCreatorCameraToolProperties> Properties;
};
