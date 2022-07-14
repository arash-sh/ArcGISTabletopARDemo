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

#include "ArcGISMapCreatorSettingsTool.generated.h"

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorSettingsToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	// UInteractiveToolBuilder
	bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
	void PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const override;
};

UCLASS(Transient)
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorSettingsToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = EditorMode)
	bool EnableEditorMode = true;

	UPROPERTY(EditAnywhere, Category = Physics)
	bool EnableMeshColliders = true;

	UPROPERTY(EditAnywhere, Category = GeoReferencingSystem)
	bool UpdateGeoReferencingSystem = true;
};

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorSettingsTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	// UInteractiveTool
	void Setup() override;
	void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

	UPROPERTY()
	TObjectPtr<UArcGISMapCreatorSettingsToolProperties> Properties;
};
