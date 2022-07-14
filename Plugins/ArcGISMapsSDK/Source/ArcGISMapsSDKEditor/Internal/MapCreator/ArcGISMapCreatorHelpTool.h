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

#include "ArcGISMapCreatorHelpTool.generated.h"

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorHelpToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

UCLASS(Transient)
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorHelpToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()
};

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorHelpTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	virtual void Setup() override;

	UPROPERTY()
	TObjectPtr<UArcGISMapCreatorHelpToolProperties> Properties;
};
