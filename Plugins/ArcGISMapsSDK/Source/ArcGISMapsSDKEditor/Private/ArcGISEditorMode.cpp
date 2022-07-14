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

#include "ArcGISEditorMode.h"

#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "Modules/ModuleManager.h"

#include "ArcGISEditorModeCommands.h"
#include "ArcGISEditorModeToolkit.h"

#include "MapCreator/ArcGISMapCreatorAuthenticationTool.h"
#include "MapCreator/ArcGISMapCreatorBasemapTool.h"
#include "MapCreator/ArcGISMapCreatorCameraTool.h"
#include "MapCreator/ArcGISMapCreatorElevationTool.h"
#include "MapCreator/ArcGISMapCreatorHelpTool.h"
#include "MapCreator/ArcGISMapCreatorLayerTool.h"
#include "MapCreator/ArcGISMapCreatorMapTool.h"
#include "MapCreator/ArcGISMapCreatorSettingsTool.h"
#include "MapCreator/CustomDetails/MapCreatorStyles.h"

#define LOCTEXT_NAMESPACE "ArcGISEditorMode"

const FEditorModeID UArcGISEditorMode::EM_ArcGISEditorModeId = TEXT("EM_ArcGISEditorMode");

FString UArcGISEditorMode::MapToolName = TEXT("ArcGISEditorMapTool");
FString UArcGISEditorMode::CameraToolName = TEXT("ArcGISEditorCameraTool");
FString UArcGISEditorMode::BasemapToolName = TEXT("ArcGISEditorBasemapTool");
FString UArcGISEditorMode::ElevationToolName = TEXT("ArcGISEditorElevationTool");
FString UArcGISEditorMode::LayerToolName = TEXT("ArcGISEditorLayerTool");
FString UArcGISEditorMode::AuthenticationToolName = TEXT("ArcGISEditorAuthenticationTool");
FString UArcGISEditorMode::SettingsToolName = TEXT("ArcGISEditorSettingsTool");
FString UArcGISEditorMode::HelpToolName = TEXT("ArcGISEditorHelpTool");

UArcGISEditorMode::UArcGISEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	MapCreatorStyles::Initialize();
	Info = FEditorModeInfo(UArcGISEditorMode::EM_ArcGISEditorModeId, LOCTEXT("ModeName", "ArcGIS Maps SDK"),
						   FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "ArcGISMiniIcon"), true);
}

void UArcGISEditorMode::Enter()
{
	UEdMode::Enter();

	const FArcGISEditorModeCommands& ArcGISEditorModeCommands = FArcGISEditorModeCommands::Get();

	RegisterTool(ArcGISEditorModeCommands.MapTool, MapToolName, NewObject<UArcGISMapCreatorMapToolBuilder>(this));
	RegisterTool(ArcGISEditorModeCommands.CameraTool, CameraToolName, NewObject<UArcGISMapCreatorCameraToolBuilder>(this));
	RegisterTool(ArcGISEditorModeCommands.BasemapTool, BasemapToolName, NewObject<UArcGISMapCreatorBasemapToolBuilder>(this));
	RegisterTool(ArcGISEditorModeCommands.ElevationTool, ElevationToolName, NewObject<UArcGISMapCreatorElevationToolBuilder>(this));
	RegisterTool(ArcGISEditorModeCommands.LayerTool, LayerToolName, NewObject<UArcGISMapCreatorLayerToolBuilder>(this));
	RegisterTool(ArcGISEditorModeCommands.AuthenticationTool, AuthenticationToolName, NewObject<UArcGISMapCreatorAuthenticationToolBuilder>(this));
	RegisterTool(ArcGISEditorModeCommands.SettingsTool, SettingsToolName, NewObject<UArcGISMapCreatorSettingsToolBuilder>(this));
	RegisterTool(ArcGISEditorModeCommands.HelpTool, HelpToolName, NewObject<UArcGISMapCreatorHelpToolBuilder>(this));

	GetToolManager()->SelectActiveToolType(EToolSide::Left, MapToolName);
}

void UArcGISEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FArcGISEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UArcGISEditorMode::GetModeCommands() const
{
	return FArcGISEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
