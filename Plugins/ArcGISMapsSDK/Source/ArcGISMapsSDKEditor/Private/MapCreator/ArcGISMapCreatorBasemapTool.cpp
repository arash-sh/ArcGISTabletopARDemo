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

#include "MapCreator/ArcGISMapCreatorBasemapTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

#include "ArcGISMapsSDK/API/GameEngine/Map/ArcGISBasemap.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayerCollection.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISBasemap.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMap.h"
#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

#define LOCTEXT_NAMESPACE "ArcGISMapCreatorBasemapTool"

namespace
{
static const FString URLImagery = TEXT("https://ibasemaps-api.arcgis.com/arcgis/rest/services/World_Imagery/MapServer");
static const FString URLOceans = TEXT("https://ibasemaps-api.arcgis.com/arcgis/rest/services/Ocean/World_Ocean_Base/MapServer");
} // namespace

bool UArcGISMapCreatorBasemapToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UArcGISMapCreatorBasemapToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	return NewObject<UArcGISMapCreatorBasemapTool>(SceneState.ToolManager);
}

EDefaultBaseMaps CompareBasemapeURLFromList(FString currentMapURL)
{
	if (currentMapURL == URLImagery)
	{
		return EDefaultBaseMaps::Imagery;
	}
	if (currentMapURL == URLOceans)
	{
		return EDefaultBaseMaps::Oceans;
	}
	if (currentMapURL == "")
	{
		return EDefaultBaseMaps::NoBasemapType;
	}

	return EDefaultBaseMaps::Custom;
}

void UArcGISMapCreatorBasemapToolBuilder::PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const
{
	UArcGISMapCreatorBasemapTool* basemapTool = Cast<UArcGISMapCreatorBasemapTool>(Tool);
	auto properties = basemapTool->Properties;

	UArcGISMapComponent* mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!mapComponent)
	{
		return;
	}

	auto map = mapComponent->GetMap();
	FString basemapSource{};

	if (auto basemap = map->GetBasemap())
	{
		auto baseLayers = basemap->GetBaseLayers();

		if (baseLayers->GetSize())
		{
			basemapSource = baseLayers->At(0)->GetSource();
		}
	}

	properties->SelectedBasemap = CompareBasemapeURLFromList(basemapSource);
	properties->BasemapAuthConfigID = mapComponent->GetBasemapAuthID();

	if (properties->SelectedBasemap == EDefaultBaseMaps::Custom)
	{
		properties->CustomBasemapSource = basemapSource;
		mapComponent->CustomBasemapSource = basemapSource;

		properties->CustomBasemapType = mapComponent->GetBasemapType();
		mapComponent->CustomBasemapType = mapComponent->GetBasemapType();
	}
	else
	{
		properties->CustomBasemapSource = mapComponent->CustomBasemapSource;
		properties->CustomBasemapType = mapComponent->CustomBasemapType;
	}
}

void UArcGISMapCreatorBasemapTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UArcGISMapCreatorBasemapToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UArcGISMapCreatorBasemapTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	UInteractiveTool::OnPropertyModified(PropertySet, Property);

	UArcGISMapComponent* map = MapComponentManager::GetCurrentMapComponent();

	if (!map)
	{
		return;
	}

	if (Properties->SelectedBasemap == EDefaultBaseMaps::Custom)
	{
		SetCustomBasemap(map, Properties->CustomBasemapSource, Properties->CustomBasemapType);
	}
	else if (Properties->SelectedBasemap == EDefaultBaseMaps::NoBasemapType)
	{
		map->GetMap()->SetBasemap(UArcGISBasemap::CreateArcGISBasemap());
		map->SetBasemapSourceAndType("", EBaseMapTypes::ImageLayer);
		map->MarkPackageDirty();
	}
	else
	{
		SetDefaultBasemap(map, Properties->SelectedBasemap);
	}
}

void UArcGISMapCreatorBasemapTool::SetDefaultBasemap(UArcGISMapComponent* map, EDefaultBaseMaps currentBasemap)
{
	if (!map)
	{
		return;
	}

	UArcGISBasemap* basemap;

	switch (currentBasemap)
	{
		case Oceans:
			basemap = UArcGISBasemap::CreateOceans(map->GetAPIkey());
			break;
		default:
			basemap = UArcGISBasemap::CreateImagery(map->GetAPIkey());
			break;
	}

	map->GetMap()->SetBasemap(basemap);
	auto source = basemap->GetBaseLayers()->At(0)->GetSource();
	map->SetBasemapSourceAndType(source, EBaseMapTypes::ImageLayer);
	map->MarkPackageDirty();
}

void UArcGISMapCreatorBasemapTool::SetCustomBasemap(UArcGISMapComponent* map, const FString& customBasemapSource, EBaseMapTypes customBasemapType)
{
	if (!map)
	{
		return;
	}

	map->SetBasemapSourceAndType(customBasemapSource, customBasemapType);

	map->CustomBasemapSource = customBasemapSource;
	map->CustomBasemapType = customBasemapType;
	map->MarkPackageDirty();
}

#undef LOCTEXT_NAMESPACE
