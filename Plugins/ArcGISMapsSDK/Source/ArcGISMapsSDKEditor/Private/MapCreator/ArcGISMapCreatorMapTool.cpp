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

#include "MapCreator/ArcGISMapCreatorMapTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMapType.h"

#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

#define LOCTEXT_NAMESPACE "ArcGISMapCreatorMapTool"

bool UArcGISMapCreatorMapToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UArcGISMapCreatorMapToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	return NewObject<UArcGISMapCreatorMapTool>(SceneState.ToolManager);
}

void UArcGISMapCreatorMapToolBuilder::PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const
{
	UArcGISMapCreatorMapTool* mapTool = Cast<UArcGISMapCreatorMapTool>(Tool);
	auto properties = mapTool->Properties;

	auto mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!mapComponent)
	{
		return;
	}

	properties->bIsOriginAtPlanetCenter = mapComponent->GetIsOriginAtPlanetCenter();
	properties->EnableExtent = mapComponent->GetIsExtentEnabled();
	properties->MapComponent = mapComponent;
	properties->MapExtent = mapComponent->GetExtent();
	properties->MapType = mapComponent->GetMapType() == EArcGISMapType::Global ? EViewMode::Global : EViewMode::Local;
	properties->OriginPosition = mapComponent->GetOriginPosition();
}

void UArcGISMapCreatorMapTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UArcGISMapCreatorMapToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UArcGISMapCreatorMapTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	if (!Property || !(Properties->MapComponent))
	{
		return;
	}

	if (Properties->OriginPosition != Properties->MapComponent->GetOriginPosition())
	{
		Properties->MapComponent->SetOriginPosition(Properties->OriginPosition);
	}

	if (Properties->bIsOriginAtPlanetCenter != Properties->MapComponent->GetIsOriginAtPlanetCenter())
	{
		Properties->MapComponent->SetIsOriginAtPlanetCenter(Properties->bIsOriginAtPlanetCenter);
	}

	auto mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!mapComponent)
	{
		return;
	}

	EArcGISMapType::Type mapType = Properties->MapType == EViewMode::Global ? EArcGISMapType::Global : EArcGISMapType::Local;

	mapComponent->SetMapType(mapType);

	if (Properties->MapExtent != mapComponent->GetExtent())
	{
		//Force Extent dimensions to be greater than 0
		if (Properties->MapExtent.ShapeDimensions.X < MinExtentDimension)
		{
			Properties->MapExtent.ShapeDimensions.X = MinExtentDimension;
		}
		if (Properties->MapExtent.ShapeDimensions.Y < MinExtentDimension)
		{
			Properties->MapExtent.ShapeDimensions.Y = MinExtentDimension;
		}

		mapComponent->SetExtent(Properties->MapExtent);
	}

	if (Properties->EnableExtent != mapComponent->GetIsExtentEnabled())
	{
		mapComponent->SetIsExtentEnabled(Properties->EnableExtent);
	}

	mapComponent->MarkPackageDirty();
}

#undef LOCTEXT_NAMESPACE
