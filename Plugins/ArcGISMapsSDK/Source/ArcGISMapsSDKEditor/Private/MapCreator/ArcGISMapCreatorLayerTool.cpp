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

#include "MapCreator/ArcGISMapCreatorLayerTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

#define LOCTEXT_NAMESPACE "ArcGISMapCreatorLayerTool"

bool UArcGISMapCreatorLayerToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UArcGISMapCreatorLayerToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	return NewObject<UArcGISMapCreatorLayerTool>(SceneState.ToolManager);
}

void UArcGISMapCreatorLayerToolBuilder::PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const
{
	TObjectPtr<UArcGISMapCreatorLayerTool> layerTool = Cast<UArcGISMapCreatorLayerTool>(Tool);
	auto properties = layerTool->Properties;

	auto mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!mapComponent)
	{
		return;
	}

	properties->Layers = mapComponent->GetLayers();
}

void UArcGISMapCreatorLayerTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UArcGISMapCreatorLayerToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UArcGISMapCreatorLayerTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	auto mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!mapComponent)
	{
		return;
	}

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UArcGISMapCreatorLayerToolProperties, LayerType) ||
		Property->GetFName() == GET_MEMBER_NAME_CHECKED(UArcGISMapCreatorLayerToolProperties, LayerName) ||
		Property->GetFName() == GET_MEMBER_NAME_CHECKED(UArcGISMapCreatorLayerToolProperties, LayerSource))
	{
		return;
	}

	mapComponent->SetLayers(Properties->Layers);
}

#undef LOCTEXT_NAMESPACE
