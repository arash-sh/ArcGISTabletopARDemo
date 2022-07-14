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
#include "Tools/UEdMode.h"

#include "ArcGISEditorMode.generated.h"

UCLASS()
class UArcGISEditorMode : public UEdMode
{
	GENERATED_BODY()

public:
	const static FEditorModeID EM_ArcGISEditorModeId;

	static FString MapToolName;
	static FString CameraToolName;
	static FString BasemapToolName;
	static FString ElevationToolName;
	static FString LayerToolName;
	static FString AuthenticationToolName;
	static FString SettingsToolName;
	static FString HelpToolName;

	UArcGISEditorMode();

	virtual void Enter() override;
	virtual void CreateToolkit() override;
	virtual TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetModeCommands() const override;
};
