// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#include "SampleAPIMapCreator.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "ArcGISMapsSDK/API/GameEngine/Elevation/Base/ArcGISElevationSource.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/Base/ArcGISLayer.h"
#include "ArcGISMapsSDK/API/GameEngine/View/ArcGISView.h"
#include "ArcGISMapsSDK/API/GameEngine/View/State/ArcGISElevationSourceViewState.h"
#include "ArcGISMapsSDK/API/GameEngine/View/State/ArcGISLayerViewState.h"
#include "ArcGISMapsSDK/API/GameEngine/View/State/ArcGISViewState.h"
#include "ArcGISMapsSDK/API/GameEngine/View/State/ArcGISViewStateMessage.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Elevation/ArcGISImageElevationSource.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Extent/ArcGISExtentCircle.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/ArcGIS3DObjectSceneLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/ArcGISImageLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayerCollection.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISBasemap.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMap.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMapElevation.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMapType.h"

#include "ArcGISPawn.h"


ASampleAPIMapCreator::ASampleAPIMapCreator()
{
	PrimaryActorTick.bCanEverTick = false;

	ViewStateLogging = CreateDefaultSubobject<UArcGISViewStateLoggingComponent>(TEXT("ArcGISViewStateLoggingComponent"));
}

void ASampleAPIMapCreator::OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent)
{
	AArcGISActor::OnArcGISMapComponentChanged(InMapComponent);

	if (MapComponent)
	{
		CreateArcGISMap();
	}
}

#if WITH_EDITOR
void ASampleAPIMapCreator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ASampleAPIMapCreator, APIKey))
	{
		if (MapComponent)
		{
			CreateArcGISMap();
		}
	}
}
#endif

// We create our map with this method
void ASampleAPIMapCreator::CreateArcGISMap()
{
	// Create the map document
	auto mapType = EArcGISMapType::Global;
	auto arcGISMap = UArcGISMap::CreateArcGISMapWithMapType(mapType);

	// Add a basemap
	auto arcGISBasemap = UArcGISBasemap::CreateImagery(APIKey);

	// Set the basemap
	arcGISMap->SetBasemap(arcGISBasemap);

	// Set the elevation
	auto elevationSource = UArcGISImageElevationSource::CreateArcGISImageElevationSourceWithName(
		"https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer", "Elevation", "");
	auto mapElevation = UArcGISMapElevation::CreateArcGISMapElevationWithElevationSource(elevationSource);
	arcGISMap->SetElevation(mapElevation);

	// Create layers
	auto layer_1 = UArcGISImageLayer::CreateArcGISImageLayerWithProperties(
		"https://tiles.arcgis.com/tiles/nGt4QxSblgDfeJn9/arcgis/rest/services/UrbanObservatory_NYC_TransitFrequency/MapServer",
		"NYTransitFrequencyTiles", 1.0f, true, "");
	arcGISMap->GetLayers()->Add(layer_1);

	auto layer_2 = UArcGISImageLayer::CreateArcGISImageLayerWithProperties(
		"https://tiles.arcgis.com/tiles/nGt4QxSblgDfeJn9/arcgis/rest/services/New_York_Industrial/MapServer", "NYIndustrialTiles", 1.0f, true,
		"");
	arcGISMap->GetLayers()->Add(layer_2);

	auto layer_3 = UArcGISImageLayer::CreateArcGISImageLayerWithProperties(
		"https://tiles.arcgis.com/tiles/4yjifSiIG17X0gW4/arcgis/rest/services/NewYorkCity_PopDensity/MapServer", "NYPopDensityTiles", 1.0f, true,
		"");
	arcGISMap->GetLayers()->Add(layer_3);

	auto buildingLayer = UArcGIS3DObjectSceneLayer::CreateArcGIS3DObjectSceneLayerWithProperties(
		"https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_NewYork_17/SceneServer", "NYScene", 1.0f, true, "");
	arcGISMap->GetLayers()->Add(buildingLayer);

	// This call invokes a function used by the Sample3DAttributesCreator component
	AttributeComponent = Cast<USample3DAttributesComponent>(GetComponentByClass(USample3DAttributesComponent::StaticClass()));

	if (AttributeComponent != nullptr)
	{
		AttributeComponent->Setup3DAttributes(buildingLayer);
	}

	// Remove a layer
	auto index = arcGISMap->GetLayers()->IndexOf(layer_3);
	arcGISMap->GetLayers()->Remove(index);
	// Update properties
	layer_1->SetOpacity(0.9f);
	layer_2->SetOpacity(0.6f);

	// Create extent
	if (mapType == EArcGISMapType::Local)
	{
		auto sr = UArcGISSpatialReference::WGS84();
		auto extentCenter = UArcGISPoint::CreateArcGISPointWithXYZSpatialReference(-74.054921, 40.691242, 3000, sr);
		auto extent = UArcGISExtentCircle::CreateArcGISExtentCircle(extentCenter, 100000);
		arcGISMap->SetClippingArea(extent);
	}

	// Set the initial Pawn location. This can be removed to set the AArcGISPawn`s location using the details panel
	AArcGISPawn* arcGISPawn = Cast<AArcGISPawn>(UGameplayStatics::GetActorOfClass(GetWorld(), AArcGISPawn::StaticClass()));
	auto PawnLocationComponent = Cast<UArcGISLocationComponent>(arcGISPawn->GetComponentByClass(UArcGISLocationComponent::StaticClass()));

	auto Position = UArcGISPoint::CreateArcGISPointWithXYZSpatialReference(-74.054921, 40.691242, 3000, UArcGISSpatialReference::WGS84());
	PawnLocationComponent->SetPosition(Position);

	auto Rotation = UArcGISRotation::CreateArcGISRotation(65, 0, 68);
	PawnLocationComponent->SetRotation(Rotation);

	// Create the view options config struct
	FArcGISViewOptions viewOptions{true};

	// Set the map and view options
	MapComponent->GetView()->SetViewOptions(viewOptions);
	MapComponent->SetMap(arcGISMap);
}
