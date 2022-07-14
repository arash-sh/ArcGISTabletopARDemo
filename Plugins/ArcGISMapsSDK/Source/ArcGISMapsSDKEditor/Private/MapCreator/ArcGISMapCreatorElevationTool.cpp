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

#include "MapCreator/ArcGISMapCreatorElevationTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

#define LOCTEXT_NAMESPACE "ArcGISMapCreatorElevationTool"

static const FString URLWorldElevation = TEXT("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer");

bool UArcGISMapCreatorElevationToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UArcGISMapCreatorElevationToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UArcGISMapCreatorElevationTool* NewTool = NewObject<UArcGISMapCreatorElevationTool>(SceneState.ToolManager);
	return NewTool;
}

void UArcGISMapCreatorElevationToolBuilder::PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const
{
	UArcGISMapCreatorElevationTool* elevationTool = Cast<UArcGISMapCreatorElevationTool>(Tool);
	auto properties = elevationTool->Properties;

	UArcGISMapComponent* map = MapComponentManager::GetCurrentMapComponent();

	if (!map)
	{
		return;
	}

	properties->ElevationAuthConfigID = map->GetElevationAuthID();
	auto source = map->GetElevationSource();

	if (source == URLWorldElevation)
	{
		properties->CustomElevationSource = map->CustomElevationSource;
	}
	else
	{
		properties->CustomElevationSource = source;
	}
}

bool UArcGISMapCreatorElevationToolProperties::IsDefaultElevationSource()
{
	auto map = MapComponentManager::GetCurrentMapComponent();

	if (!map)
	{
		return false;
	}

	if (map->GetIsElevationEnabled() && map->GetElevationSource() == URLWorldElevation)
	{
		EnableCustomElevation = false;
		return true;
	}

	return false;
}

bool UArcGISMapCreatorElevationToolProperties::IsCustomElevationSource()
{
	auto map = MapComponentManager::GetCurrentMapComponent();

	if (!map)
	{
		return false;
	}

	if (map->GetIsElevationEnabled() && map->GetElevationSource() != URLWorldElevation)
	{
		EnableCustomElevation = true;
		return true;
	}

	return false;
}

void UArcGISMapCreatorElevationToolProperties::SetDefaultElevation(bool isEnabled)
{
	auto map = MapComponentManager::GetCurrentMapComponent();

	if (!map)
	{
		return;
	}

	if (isEnabled)
	{
		map->SetIsElevationEnabled(true);
		map->SetElevationSource(URLWorldElevation);
	}
	else
	{
		map->SetIsElevationEnabled(false);
	}

	map->MarkPackageDirty();
}

void UArcGISMapCreatorElevationToolProperties::SetCustomElevation(bool isEnabled)
{
	auto map = MapComponentManager::GetCurrentMapComponent();

	if (!map)
	{
		return;
	}

	if (isEnabled)
	{
		map->SetIsElevationEnabled(true);
		map->SetElevationSource(CustomElevationSource);
	}
	else
	{
		map->SetIsElevationEnabled(false);
	}

	map->MarkPackageDirty();
}

void UArcGISMapCreatorElevationTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UArcGISMapCreatorElevationToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UArcGISMapCreatorElevationTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	UInteractiveTool::OnPropertyModified(PropertySet, Property);

	auto map = MapComponentManager::GetCurrentMapComponent();

	if (!map)
	{
		return;
	}

	if (Properties->EnableCustomElevation && Properties->CustomElevationSource != map->GetElevationSource())
	{
		map->SetElevationSource(Properties->CustomElevationSource);
		map->CustomElevationSource = Properties->CustomElevationSource;
		map->MarkPackageDirty();
	}

	if (Properties->ElevationAuthConfigID != map->ElevationAuthConfigID)
	{
		map->SetElevationAuthID(Properties->ElevationAuthConfigID);
		map->MarkPackageDirty();
	}
}

#undef LOCTEXT_NAMESPACE
