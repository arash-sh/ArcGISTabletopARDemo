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

#include "ArcGISMapsSDK/Utils/AuthGuid.h"

#include "ArcGISMapCreatorElevationTool.generated.h"

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorElevationToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	// UInteractiveToolBuilder
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
	virtual void PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const override;
};

UCLASS(Transient)
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorElevationToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Elevation, meta = (DisplayName = "Source"))
	FString CustomElevationSource;

	UPROPERTY(EditAnywhere, Category = Elevation, meta = (DisplayName = "Authentication", ElevationAuth = true))
	FAuthGuid ElevationAuthConfigID;

	bool EnableCustomElevation{false};

	bool IsDefaultElevationSource();
	bool IsCustomElevationSource();
	void SetDefaultElevation(bool isEnabled);
	void SetCustomElevation(bool isEnabled);
};

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorElevationTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	// UInteractiveTool
	virtual void Setup() override;
	virtual void OnPropertyModified(UObject* PropertySet, FProperty* Property) override;

	UPROPERTY()
	TObjectPtr<UArcGISMapCreatorElevationToolProperties> Properties;
};
