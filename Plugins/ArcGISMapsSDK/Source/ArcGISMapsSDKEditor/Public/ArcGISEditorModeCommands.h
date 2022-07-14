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
#include "Framework/Commands/Commands.h"

class FArcGISEditorModeCommands : public TCommands<FArcGISEditorModeCommands>
{
public:
	FArcGISEditorModeCommands();

	virtual void RegisterCommands() override;
	static TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands();

	TSharedPtr<FUICommandInfo> MapTool;
	TSharedPtr<FUICommandInfo> CameraTool;
	TSharedPtr<FUICommandInfo> BasemapTool;
	TSharedPtr<FUICommandInfo> ElevationTool;
	TSharedPtr<FUICommandInfo> LayerTool;
	TSharedPtr<FUICommandInfo> AuthenticationTool;
	TSharedPtr<FUICommandInfo> SettingsTool;
	TSharedPtr<FUICommandInfo> HelpTool;

protected:
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
};
