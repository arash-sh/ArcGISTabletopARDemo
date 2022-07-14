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

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

#include "ArcGISMapCreatorBasemapTool.generated.h"

UENUM()
enum EDefaultBaseMaps
{
	Imagery,
	Oceans,
	Custom,
	NoBasemapType,
};

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorBasemapToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
	virtual void PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const override;
};

UCLASS(Transient)
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorBasemapToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Basemap, meta = (DisplayName = "Basemaps"))
	TEnumAsByte<EDefaultBaseMaps> SelectedBasemap;

	UPROPERTY(EditAnywhere, Category = Basemap, meta = (DisplayName = "Source"))
	FString CustomBasemapSource;

	UPROPERTY(EditAnywhere, Category = Basemap, meta = (DisplayName = "Type"))
	EBaseMapTypes CustomBasemapType = EBaseMapTypes::ImageLayer;

	UPROPERTY(EditAnywhere, Category = Basemap, meta = (DisplayName = "Authentication", BasemapAuth = true))
	FAuthGuid BasemapAuthConfigID;
};

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorBasemapTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	virtual void Setup() override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

	UPROPERTY()
	TObjectPtr<UArcGISMapCreatorBasemapToolProperties> Properties;

	static void SetDefaultBasemap(UArcGISMapComponent* map, EDefaultBaseMaps currentBasemap);
	static void SetCustomBasemap(UArcGISMapComponent* map, const FString& customBasemapSource, EBaseMapTypes customBasemapType);
};
