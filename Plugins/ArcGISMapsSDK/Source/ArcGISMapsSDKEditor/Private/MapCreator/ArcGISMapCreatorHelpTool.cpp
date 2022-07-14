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

#include "MapCreator/ArcGISMapCreatorHelpTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

#define LOCTEXT_NAMESPACE "ArcGISMapCreatorHelpTool"

bool UArcGISMapCreatorHelpToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UArcGISMapCreatorHelpToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UArcGISMapCreatorHelpTool* NewTool = NewObject<UArcGISMapCreatorHelpTool>(SceneState.ToolManager);
	return NewTool;
}

void UArcGISMapCreatorHelpTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UArcGISMapCreatorHelpToolProperties>(this);
	AddToolPropertySource(Properties);
}

#undef LOCTEXT_NAMESPACE
