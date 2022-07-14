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

#include "ArcGISEditorModeCommands.h"

#include "EditorStyleSet.h"

#include "ArcGISEditorMode.h"
#include "MapCreator/CustomDetails/MapCreatorStyles.h"

#define LOCTEXT_NAMESPACE "ArcGISEditorModeCommands"

FArcGISEditorModeCommands::FArcGISEditorModeCommands()
: TCommands<FArcGISEditorModeCommands>("ArcGISEditorMode",
									   NSLOCTEXT("ArcGISEditorMode", "ArcGISEditorModeCommands", "ArcGIS Editor Mode"),
									   NAME_None,
									   FEditorStyle::GetStyleSetName())
{
}

void FArcGISEditorModeCommands::RegisterCommands()
{
	TArray<TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	FSlateIcon MapIcon = FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "MapIcon");
	FSlateIcon CameraIcon = FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "CameraIcon");
	FSlateIcon BasemapIcon = FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "BasemapIcon");
	FSlateIcon ElevationIcon = FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "ElevationIcon");
	FSlateIcon LayerIcon = FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "LayersIcon");
	FSlateIcon AuthIcon = FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "AuthIcon");
	FSlateIcon SettingsIcon = FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "SettingsIcon");
	FSlateIcon HelpIcon = FSlateIcon(MapCreatorStyles::GetCommonStyleSet()->GetStyleSetName(), "HelpIcon");

	FUICommandInfo::MakeCommandInfo(this->AsShared(), MapTool, FName("Map"), FText::FromString("Map"),
									FText::FromString("Create a new ArcGIS Map setup"), MapIcon, EUserInterfaceActionType::RadioButton,
									FInputChord());

	FUICommandInfo::MakeCommandInfo(this->AsShared(), CameraTool, FName("Camera"), FText::FromString("Camera"),
									FText::FromString("Change camera settings"), CameraIcon, EUserInterfaceActionType::RadioButton, FInputChord());

	FUICommandInfo::MakeCommandInfo(this->AsShared(), BasemapTool, FName("Basemap"), FText::FromString("Basemap"),
									FText::FromString("Change basemap settings"), BasemapIcon, EUserInterfaceActionType::RadioButton, FInputChord());

	FUICommandInfo::MakeCommandInfo(this->AsShared(), ElevationTool, FName("Elevation"), FText::FromString("Elevation"),
									FText::FromString("Change elevation settings"), ElevationIcon, EUserInterfaceActionType::RadioButton,
									FInputChord());

	FUICommandInfo::MakeCommandInfo(this->AsShared(), LayerTool, FName("Layers"), FText::FromString("Layers"),
									FText::FromString("Add and change map content"), LayerIcon, EUserInterfaceActionType::RadioButton, FInputChord());

	FUICommandInfo::MakeCommandInfo(this->AsShared(), AuthenticationTool, FName("Auth"), FText::FromString("Auth"),
									FText::FromString("Use authentication services"), AuthIcon, EUserInterfaceActionType::RadioButton, FInputChord());

	FUICommandInfo::MakeCommandInfo(this->AsShared(), SettingsTool, FName("Settings"), FText::FromString("Settings"),
									FText::FromString("Plugin settings"), SettingsIcon, EUserInterfaceActionType::RadioButton, FInputChord());

	FUICommandInfo::MakeCommandInfo(this->AsShared(), HelpTool, FName("Help"), FText::FromString("Help"),
									FText::FromString("View help services and links"), HelpIcon, EUserInterfaceActionType::RadioButton, FInputChord());

	ToolCommands.Add(MapTool);
	ToolCommands.Add(CameraTool);
	ToolCommands.Add(BasemapTool);
	ToolCommands.Add(ElevationTool);
	ToolCommands.Add(LayerTool);
	ToolCommands.Add(AuthenticationTool);
	ToolCommands.Add(SettingsTool);
	ToolCommands.Add(HelpTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FArcGISEditorModeCommands::GetCommands()
{
	return FArcGISEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
