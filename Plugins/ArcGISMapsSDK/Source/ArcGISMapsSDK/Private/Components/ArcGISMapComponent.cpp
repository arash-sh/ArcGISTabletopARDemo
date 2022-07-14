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
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

#include "Async/Async.h"
#include "GameFramework/PlayerController.h"
#include "Math/UnrealMathUtility.h"

#include "ArcGISMapsSDK/API/GameEngine/ArcGISLoadStatus.h"
#include "ArcGISMapsSDK/API/GameEngine/Elevation/Base/ArcGISElevationSource.h"
#include "ArcGISMapsSDK/API/GameEngine/Extent/ArcGISExtentCircle.h"
#include "ArcGISMapsSDK/API/GameEngine/Extent/ArcGISExtentRectangle.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISSpheroidData.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/Base/ArcGISLayer.h"
#include "ArcGISMapsSDK/API/GameEngine/Map/ArcGISBasemap.h"
#include "ArcGISMapsSDK/API/GameEngine/Map/ArcGISMap.h"
#include "ArcGISMapsSDK/API/GameEngine/Map/ArcGISMapElevation.h"
#include "ArcGISMapsSDK/API/GameEngine/Map/ArcGISMapInternal.h"
#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationConfiguration.h"
#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationManager.h"
#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISOAuthAuthenticationConfiguration.h"
#include "ArcGISMapsSDK/API/GameEngine/View/ArcGISView.h"
#include "ArcGISMapsSDK/API/GameEngine/View/State/ArcGISElevationSourceViewState.h"
#include "ArcGISMapsSDK/API/GameEngine/View/State/ArcGISLayerViewState.h"
#include "ArcGISMapsSDK/API/GameEngine/View/State/ArcGISViewState.h"
#include "ArcGISMapsSDK/API/GameEngine/View/State/ArcGISViewStateMessage.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISCollection.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISDictionary.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Elevation/ArcGISImageElevationSource.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Extent/ArcGISExtent.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Extent/ArcGISExtentCircle.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Extent/ArcGISExtentRectangle.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/ArcGIS3DObjectSceneLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/ArcGISImageLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/ArcGISIntegratedMeshLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayerCollection.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISBasemap.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMapElevation.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/View/ArcGISViewOptions.h"
#include "ArcGISMapsSDK/Components/ArcGISLocationComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISRendererComponent.h"
#if PLATFORM_ANDROID
#include "ArcGISMapsSDK/Memory/ArcGISAndroidDefaultMemorySystemHandler.h"
#else
#include "ArcGISMapsSDK/Memory/ArcGISDefaultMemorySystemHandler.h"
#endif
#include "ArcGISMapsSDK/Subsystem/ArcGISMapsSDKSubsystem.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoUtils.h"

namespace
{
bool CanCreateArcGISLayerFromDefinition(const FArcGISLayerInstanceData& LayerDefinition)
{
	return LayerDefinition.Source != "" &&
		   (LayerDefinition.Type == EArcGISLayerType::ArcGIS3DObjectSceneLayer || LayerDefinition.Type == EArcGISLayerType::ArcGISImageLayer ||
			LayerDefinition.Type == EArcGISLayerType::ArcGISIntegratedMeshLayer);
}

UArcGISLayer* CreateArcGISLayerFromDefinition(const FArcGISLayerInstanceData& LayerDefinition, const FString& APIKey)
{
	UArcGISLayer* Layer{nullptr};

	if (LayerDefinition.Source != "")
	{
		if (LayerDefinition.Type == EArcGISLayerType::ArcGIS3DObjectSceneLayer)
		{
			Layer = UArcGIS3DObjectSceneLayer::CreateArcGIS3DObjectSceneLayerWithProperties(LayerDefinition.Source, LayerDefinition.Name,
																							LayerDefinition.Opacity, true, APIKey);
		}
		else if (LayerDefinition.Type == EArcGISLayerType::ArcGISImageLayer)
		{
			Layer = UArcGISImageLayer::CreateArcGISImageLayerWithProperties(LayerDefinition.Source, LayerDefinition.Name, LayerDefinition.Opacity,
																			true, APIKey);
		}
		else if (LayerDefinition.Type == EArcGISLayerType::ArcGISIntegratedMeshLayer)
		{
			Layer = UArcGISIntegratedMeshLayer::CreateArcGISIntegratedMeshLayerWithProperties(LayerDefinition.Source, LayerDefinition.Name,
																							  LayerDefinition.Opacity, true, APIKey);
		}
	}

	return Layer;
}

UArcGISExtent* CreateArcGISExtentFromDefinition(FArcGISExtentInstanceData& Extent)
{
	auto center =
		UArcGISPoint::CreateArcGISPointWithXYZSpatialReference(Extent.ExtentCenter.X, Extent.ExtentCenter.Y, Extent.ExtentCenter.Z,
															   UArcGISSpatialReference::CreateArcGISSpatialReference(Extent.ExtentCenter.GetWKID()));

	UArcGISExtent* extent{nullptr};

	switch (Extent.ExtentShape)
	{
		case EMapExtentShapes::Rectangle:
			extent = UArcGISExtentRectangle::CreateArcGISExtentRectangle(center, Extent.ShapeDimensions.X, Extent.ShapeDimensions.Y);
			break;
		case EMapExtentShapes::Square:
			extent = UArcGISExtentRectangle::CreateArcGISExtentRectangle(center, Extent.ShapeDimensions.X, Extent.ShapeDimensions.X);
			break;
		case EMapExtentShapes::Circle:
			extent = UArcGISExtentCircle::CreateArcGISExtentCircle(center, Extent.ShapeDimensions.X);
			break;
	}

	return extent;
}

bool IsExtentDefinitionValid(FArcGISExtentInstanceData& Extent)
{
	if (Extent.ShapeDimensions.X < 0)
	{
		Extent.ShapeDimensions.X = 0;
	}

	if (Extent.ShapeDimensions.Y < 0)
	{
		Extent.ShapeDimensions.Y = 0;
	}

	return Extent.ShapeDimensions.X > 0 && (Extent.ExtentShape != EMapExtentShapes::Rectangle || Extent.ShapeDimensions.Y > 0);
}
} // namespace

bool operator==(const FArcGISLayerInstanceData& Left, const FArcGISLayerInstanceData& Right)
{
	return Left.ID == Right.ID;
}

bool operator!=(const FArcGISLayerInstanceData& Left, const FArcGISLayerInstanceData& Right)
{
	return !(Left == Right);
}

void UArcGISMapComponent::ApplyMapTypeUpdate()
{
	ResetMap();

	SetMapToView();

	PushToGeoReferencingSystem();
}

void UArcGISMapComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	Initialize();

	PullFromGeoReferencingSystem();
}

FString UArcGISMapComponent::GetAPIkey() const
{
	return APIKey;
}

void UArcGISMapComponent::PullFromGeoReferencingSystem()
{
	if (UWorld* OwningWorld = GetWorld())
	{
		if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
		{
			if (auto geoReferencingSystem = arcGISMapsSDKSubsystem->GetGeoReferencingSystem())
			{
				double X;
				double Y;
				double Z;

				FString spatialReference;

				bIsOriginAtPlanetCenter = geoReferencingSystem->bOriginAtPlanetCenter;

				if (!geoReferencingSystem->bOriginLocationInProjectedCRS)
				{
					X = geoReferencingSystem->OriginLongitude;
					Y = geoReferencingSystem->OriginLatitude;
					Z = geoReferencingSystem->OriginAltitude;

					spatialReference = geoReferencingSystem->GeographicCRS;
				}
				else
				{
					X = geoReferencingSystem->OriginProjectedCoordinatesEasting;
					Y = geoReferencingSystem->OriginProjectedCoordinatesNorthing;
					Z = geoReferencingSystem->OriginProjectedCoordinatesUp;

					spatialReference = geoReferencingSystem->ProjectedCRS;
				}

				spatialReference = spatialReference.Replace(TEXT("EPSG:"), TEXT(""));

				SetMapType(geoReferencingSystem->PlanetShape == EPlanetShape::FlatPlanet ? EArcGISMapType::Local : EArcGISMapType::Global);
				SetOriginPosition(FGeoPosition(X, Y, Z, Esri::GameEngine::Geometry::ArcGISSpatialReference(FCString::Atoi(*spatialReference))));
			}
		}
	}
}

void UArcGISMapComponent::PushToGeoReferencingSystem()
{
	if (bUpdateGeoReferencingSystem && GeoReferencingSystem)
	{
		auto spatialReference = Esri::GameEngine::Geometry::ArcGISSpatialReference(OriginPosition.GetWKID());

		if (!spatialReference.GetIsGeographic())
		{
			GeoReferencingSystem->OriginProjectedCoordinatesEasting = OriginPosition.X;
			GeoReferencingSystem->OriginProjectedCoordinatesNorthing = OriginPosition.Y;
			GeoReferencingSystem->OriginProjectedCoordinatesUp = OriginPosition.Z;

			GeoReferencingSystem->ProjectedCRS = "EPSG:" + FString::FromInt(OriginPosition.GetWKID());
		}
		else
		{
			GeoReferencingSystem->OriginLongitude = OriginPosition.X;
			GeoReferencingSystem->OriginLatitude = OriginPosition.Y;
			GeoReferencingSystem->OriginAltitude = OriginPosition.Z;

			GeoReferencingSystem->GeographicCRS = "EPSG:" + FString::FromInt(OriginPosition.GetWKID());
		}

		GeoReferencingSystem->bOriginAtPlanetCenter = bIsOriginAtPlanetCenter;
		GeoReferencingSystem->bOriginLocationInProjectedCRS = !spatialReference.GetIsGeographic();

		if (GetMapType() == EArcGISMapType::Local)
		{
			GeoReferencingSystem->PlanetShape = EPlanetShape::FlatPlanet;
		}
		else
		{
			GeoReferencingSystem->PlanetShape = EPlanetShape::RoundPlanet;
		}

		GeoReferencingSystem->ApplySettings();
	}
}

void UArcGISMapComponent::SetAPIkey(const FString& InAPIkey)
{
	if (APIKey == InAPIkey)
	{
		return;
	}

	APIKey = InAPIkey;

	ApplyAPIKeyUpdate();
}

void UArcGISMapComponent::SetAreMeshCollidersEnabled(bool bInAreMeshCollidersEnabled)
{
	bAreMeshCollidersEnabled = bInAreMeshCollidersEnabled;

	OnMeshCollidersEnabledChanged.Broadcast();
}

void UArcGISMapComponent::SetBasemapSourceAndType(const FString& InBasemapSource, EBaseMapTypes InBasemapType)
{
	if (Basemap == InBasemapSource && BasemapType == InBasemapType)
	{
		return;
	}

	Basemap = InBasemapSource;
	BasemapType = InBasemapType;

	ApplyBasemapUpdate();
}

void UArcGISMapComponent::UpdateENUToWorldTransform()
{
	using namespace Esri::ArcGISMapsSDK::Utils::GeoCoord;

	if (!HasSpatialReference())
	{
		return;
	}

	const auto& ViewAPI = GetView()->APIObject;
	{
		auto TransformedOrigin = ViewAPI->GeographicToWorld(OriginPosition);
		auto Origin = FVector3d(TransformedOrigin.X, TransformedOrigin.Y, TransformedOrigin.Z);

		if (FMath::IsNaN(Origin.X) || FMath::IsNaN(Origin.Y) || FMath::IsNaN(Origin.Z))
		{
			return;
		}

		switch (GetMapType())
		{
			case EArcGISMapType::Global:
			{
				if (bIsOriginAtPlanetCenter)
				{
					ENUFrameToWorldFrame = FMatrix::Identity;
					WorldFrameToENUFrame = FMatrix::Identity;
				}
				else
				{
					ENUFrameToWorldFrame = GeoUtils::GetENUReferenceMatrix(Origin, ViewAPI->GetSpatialReference().GetSpheroidData());
					WorldFrameToENUFrame = ENUFrameToWorldFrame.Inverse();
				}
			}
			break;

			case EArcGISMapType::Local:
			default:
			{
				// clang-format off
				ENUFrameToWorldFrame = FMatrix44d(FPlane4d(1.0, 0.0, 0.0, 0.0),
												  FPlane4d(0.0, 1.0, 0.0, 0.0),
												  FPlane4d(0.0, 0.0, 1.0, 0.0),
												  FPlane4d(Origin, 1.0));
				// clang-format on
				WorldFrameToENUFrame = ENUFrameToWorldFrame.Inverse();
			}
			break;
		}
	}
}

const FString& UArcGISMapComponent::GetBasemapSource() const
{
	return Basemap;
}

EBaseMapTypes UArcGISMapComponent::GetBasemapType() const
{
	return BasemapType;
}

FAuthGuid UArcGISMapComponent::GetBasemapAuthID() const
{
	return BasemapAuthConfigID;
}

void UArcGISMapComponent::SetBasemapSource(const FString& InBasemapSource)
{
	if (Basemap == InBasemapSource)
	{
		return;
	}

	Basemap = InBasemapSource;

	ApplyBasemapUpdate();
}

void UArcGISMapComponent::SetBasemapAuthID(FAuthGuid InBasemapAuthID)
{
	BasemapAuthConfigID = std::move(InBasemapAuthID);
}

void UArcGISMapComponent::SetBasemapType(EBaseMapTypes InBasemapType)
{
	if (BasemapType == InBasemapType)
	{
		return;
	}

	BasemapType = InBasemapType;

	ApplyBasemapUpdate();
}

bool UArcGISMapComponent::GetIsElevationEnabled() const
{
	return EnableElevation;
}

void UArcGISMapComponent::SetIsEditorModeEnabled(bool bInIsEditorModeEnabled)
{
	if (EnableElevation == bInIsEditorModeEnabled)
	{
		return;
	}

	EnableElevation = bInIsEditorModeEnabled;

	ApplyElevationUpdate();
}

FString UArcGISMapComponent::GetElevationSource() const
{
	return ElevationSource;
}

void UArcGISMapComponent::SetElevationSource(const FString& InElevationSource)
{
	if (ElevationSource == InElevationSource)
	{
		return;
	}

	ElevationSource = InElevationSource;

	ApplyElevationUpdate();
}

FAuthGuid UArcGISMapComponent::GetElevationAuthID() const
{
	return ElevationAuthConfigID;
}

void UArcGISMapComponent::SetElevationAuthID(FAuthGuid InElevationAuthID)
{
	ElevationAuthConfigID = std::move(InElevationAuthID);
}

const FArcGISExtentInstanceData& UArcGISMapComponent::GetExtent() const
{
	return Extent;
}

bool UArcGISMapComponent::GetIsEditorModeEnabled() const
{
	return bIsEditorModeEnabled;
}

bool UArcGISMapComponent::GetIsOriginAtPlanetCenter() const
{
	return bIsOriginAtPlanetCenter;
}

void UArcGISMapComponent::SetExtent(const FArcGISExtentInstanceData& InExtent)
{
	if (Extent == InExtent)
	{
		return;
	}

	Extent = InExtent;

	ApplyExtentUpdate();
}

void UArcGISMapComponent::SetIsElevationEnabled(bool bInIsEditorModeEnabled)
{
	if (bIsEditorModeEnabled == bInIsEditorModeEnabled)
	{
		return;
	}

	bIsEditorModeEnabled = bInIsEditorModeEnabled;

	ApplyEditorModeUpdate();
}

void UArcGISMapComponent::SetIsOriginAtPlanetCenter(bool bInIsOriginAtPlanetCenter)
{
	if (bIsOriginAtPlanetCenter == bInIsOriginAtPlanetCenter)
	{
		return;
	}

	bIsOriginAtPlanetCenter = bInIsOriginAtPlanetCenter;

	UpdateENUToWorldTransform();
	PushToGeoReferencingSystem();
}

bool UArcGISMapComponent::GetIsExtentEnabled() const
{
	return EnableExtent;
}

FTransform UArcGISMapComponent::GetENUAtLocationToViewENUTransform(const FVector3d& InEngineCoordinates)
{
	using namespace Esri::ArcGISMapsSDK::Utils::GeoCoord;

	const auto& ViewAPI = GetView()->APIObject;
	{
		auto mapType = GetMapType();

		auto SpatialReference = ViewAPI->GetSpatialReference();
		auto WorldPosition = FromEnginePosition(InEngineCoordinates);

		FMatrix44d ENUAtLocationToWorld;

		if (mapType == EArcGISMapType::Local || !SpatialReference)
		{
			// clang-format off
			ENUAtLocationToWorld = FMatrix44d(FPlane4d(1.0, 0.0, 0.0, 0.0),
										 	  FPlane4d(0.0, 1.0, 0.0, 0.0),
										 	  FPlane4d(0.0, 0.0, 1.0, 0.0),
										 	  FPlane4d(WorldPosition, 1.0));
			// clang-format on
		}
		else
		{
			ENUAtLocationToWorld = GeoUtils::GetENUReferenceMatrix(WorldPosition, SpatialReference.GetSpheroidData());
		}

		// Transformations are applied from left to right
		FMatrix44d ENUAtLocationToViewENU = FromUnrealCoordSystem * ENUAtLocationToWorld * WorldFrameToENUFrame * ToUnrealCoordSystem;

		FVector3d East, North, Up;
		ENUAtLocationToViewENU.GetScaledAxes(East, North, Up);

		FMatrix ResultMatrix = FMatrix::Identity;
		ResultMatrix.SetAxes(&East, &North, &Up, &InEngineCoordinates);
		return FTransform(ResultMatrix);
	}
}

FTransform UArcGISMapComponent::GetENUToWorldTransformAtOrigin()
{
	FMatrix44d WorldToENU = FromUnrealCoordSystem * WorldFrameToENUFrame * ToUnrealCoordSystem;
	return FTransform(WorldToENU);
}

void UArcGISMapComponent::SetIsExtentEnabled(bool bInIsExtentEnabled)
{
	if (EnableExtent == bInIsExtentEnabled)
	{
		return;
	}

	EnableExtent = bInIsExtentEnabled;

	ApplyExtentUpdate();
}

void UArcGISMapComponent::SetMapType(TEnumAsByte<EArcGISMapType::Type> InMapType)
{
	if (MapType == InMapType)
	{
		return;
	}

	MapType = InMapType;

	ApplyMapTypeUpdate();
}

void UArcGISMapComponent::SetMemoryQuotas(FArcGISMemoryQuotas InMemoryQuotas)
{
	UE_LOG(LogTemp, Display, TEXT("Setting System Memory Quota: %lld MiB"), InMemoryQuotas.SystemMemory);
	UE_LOG(LogTemp, Display, TEXT("Setting Video Memory Quota: %lld MiB"), InMemoryQuotas.VideoMemory);

	GetView()->SetMemoryQuotas(InMemoryQuotas.SystemMemory, InMemoryQuotas.VideoMemory);
}

void UArcGISMapComponent::SetMemorySystemHandler(IArcGISMemorySystemHandler* InMemorySystemHandler)
{
	if (MemorySystemHandler == InMemorySystemHandler)
	{
		return;
	}

	MemorySystemHandler = InMemorySystemHandler;

	if (MemorySystemHandler)
	{
		InitializeMemorySystem();
	}
}

void UArcGISMapComponent::SetOriginPosition(const FGeoPosition& InOriginPosition)
{
	if (OriginPosition == InOriginPosition)
	{
		return;
	}

	OriginPosition = InOriginPosition;

	UpdateENUToWorldTransform();
	PushToGeoReferencingSystem();

	OnOriginPositionChanged.Broadcast();
}

void UArcGISMapComponent::SetShouldUpdateGeoReferencingSystem(bool InShouldUpdateGeoReferencingSystem)
{
	if (bUpdateGeoReferencingSystem == InShouldUpdateGeoReferencingSystem)
	{
		return;
	}

	bUpdateGeoReferencingSystem = InShouldUpdateGeoReferencingSystem;

	PushToGeoReferencingSystem();
}

void UArcGISMapComponent::SetViewportProperties(int32 InViewportWidthPixels,
												int32 InViewportHeightPixels,
												float InHorizontalFieldOfViewDegrees,
												float InVerticalFieldOfViewDegrees,
												float InVerticalDistortionFactor)
{
	CurrentViewportProperties = {InViewportWidthPixels, InViewportHeightPixels, InHorizontalFieldOfViewDegrees, InVerticalFieldOfViewDegrees,
								 InVerticalDistortionFactor};
	GetView()->SetViewportProperties(InViewportWidthPixels, InViewportHeightPixels, InHorizontalFieldOfViewDegrees, InVerticalFieldOfViewDegrees,
									 InVerticalDistortionFactor);
}

bool UArcGISMapComponent::ShouldEditorComponentBeUpdated() const
{
#if WITH_EDITOR
	if (GetWorld())
	{
		return GetWorld()->WorldType == EWorldType::PIE || (bIsEditorModeEnabled && GetWorld()->WorldType == EWorldType::Editor);
	}
	else
	{
		return false;
	}
#else
	return true;
#endif
}

FVector3d UArcGISMapComponent::ToEnginePosition(const FVector3d& InWorldPosition)
{
	FVector3d ENUFrameCoordinates = WorldFrameToENUFrame.TransformPosition(InWorldPosition);

	// Convert UE units to centimeters and flip the Y axis to match the GeoReferencing plugin convention
	FVector3d EngineCoordinates3d = ToUnrealCoordSystem.TransformPosition(ENUFrameCoordinates);

	// Consider UE's world offset
	FIntVector WorldOriginOffset = GetWorld()->OriginLocation;
	return EngineCoordinates3d - FVector3d(WorldOriginOffset.X, WorldOriginOffset.Y, WorldOriginOffset.Z);
}

const TArray<FArcGISLayerInstanceData>& UArcGISMapComponent::GetLayers() const
{
	return Layers;
}

UArcGISMap* UArcGISMapComponent::GetMap()
{
	return GetView()->GetMap();
}

UArcGISMapComponent* UArcGISMapComponent::GetMapComponent(UObject* InWorldContextObject)
{
	auto World = GEngine->GetWorldFromContextObject(InWorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	auto arcGISMapsSDKSubsystem = World->GetSubsystem<UArcGISMapsSDKSubsystem>();
	check(arcGISMapsSDKSubsystem);

	return arcGISMapsSDKSubsystem->GetMapComponent();
}

TEnumAsByte<EArcGISMapType::Type> UArcGISMapComponent::GetMapType() const
{
	return MapType;
}

IArcGISMemorySystemHandler* UArcGISMapComponent::GetMemorySystemHandler()
{
	if (!MemorySystemHandler)
	{
#if PLATFORM_ANDROID
		MemorySystemHandler = new FArcGISAndroidDefaultMemorySystemHandler();
#else
		MemorySystemHandler = new FArcGISDefaultMemorySystemHandler();
#endif
	}

	return MemorySystemHandler;
}

FGeoPosition UArcGISMapComponent::GetOriginPosition() const
{
	return OriginPosition;
}

bool UArcGISMapComponent::GetShouldUpdateGeoReferencingSystem() const
{
	return bUpdateGeoReferencingSystem;
}

UArcGISView* UArcGISMapComponent::GetView()
{
	if (!View)
	{
		View = TStrongObjectPtr<UArcGISView>(UArcGISView::CreateArcGISView(EArcGISGameEngineType::UnrealEngine, EArcGISGlobeModel::Ellipsoid));

		TWeakObjectPtr<UArcGISMapComponent> Self = this;
		View->APIObject->SetSpatialReferenceChanged([Self]() {
			AsyncTask(ENamedThreads::GameThread, [Self]() {
				auto MapComponent = Self.Get();

				// It may happen the UArcGISMapComponent gets destroyed in-between
				if (MapComponent)
				{
					Self->UpdateENUToWorldTransform();
					Self->OnSpatialReferenceChanged.Broadcast();
				}
			});
		});

		InitializeMemorySystem();
		UpdateENUToWorldTransform();
	}

	return View.Get();
}

FViewportProperties UArcGISMapComponent::GetViewportProperties() const
{
	return CurrentViewportProperties;
}

bool UArcGISMapComponent::HasSpatialReference()
{
	const auto& ViewAPI = GetView()->APIObject;
	{
		return ViewAPI->GetSpatialReference().GetHandle() != nullptr;
	}
}

void UArcGISMapComponent::Initialize()
{
	// clang-format off
	ToUnrealCoordSystem = FMatrix44d(FPlane4d(100.0, 0.0, 0.0, 0.0),
									 FPlane4d(0.0, 100.0, 0.0, 0.0),
									 FPlane4d(0.0, 0.0, 100.0, 0.0),
									 FPlane4d(0.0, 0.0, 0.0, 1.0));
	// clang-format on
	FromUnrealCoordSystem = ToUnrealCoordSystem.Inverse();
}

void UArcGISMapComponent::SetLayers(TArray<FArcGISLayerInstanceData> InLayers)
{
	Layers = std::move(InLayers);

	UpdateLayers();
}

void UArcGISMapComponent::SetMap(UArcGISMap* InMap)
{
	Map.Reset(InMap);

	SetMapToView();

	if (InMap)
	{
		MapType = InMap->GetMapType();

		UpdateENUToWorldTransform();
		PushToGeoReferencingSystem();

		OnMapTypeChanged.Broadcast();
	}
}

const TArray<FArcGISAuthenticationConfigurationInstanceData>& UArcGISMapComponent::GetAuthenticationConfigurations() const
{
	return AuthenticationConfigurations;
}

void UArcGISMapComponent::SetAuthenticationConfigurations(TArray<FArcGISAuthenticationConfigurationInstanceData> InAuthenticationConfigurations)
{
	AuthenticationConfigurations = std::move(InAuthenticationConfigurations);

	UpdateAuthenticationConfigurations();
}

void UArcGISMapComponent::SetMapToView()
{
	if (ShouldEditorComponentBeUpdated())
	{
		GetView()->SetMap(Map.Get());
	}
}

FArcGISRaycastHit UArcGISMapComponent::GetArcGISRaycastHit(FHitResult InHitResult)
{
	FArcGISRaycastHit output;
	output.FeatureId = -1;

	if (InHitResult.FaceIndex == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetArcGISRaycastHit requires a valid FaceIndex to function correctly"));
		return output;
	}


	auto ArcGISRendererComponent = Cast<UArcGISRendererComponent>(GetOwner()->GetComponentByClass(UArcGISRendererComponent::StaticClass()));
	auto MeshComponent = Cast<UMeshComponent>(InHitResult.GetComponent());

	if (MeshComponent && ArcGISRendererComponent && MapComponent.IsValid())
	{
		auto LayerId = ArcGISRendererComponent->GetLayerIdByMeshComponent(MeshComponent);
		output.FeatureId = FRaycastHelpers::GetFeatureIndexByTriangleIndex(MeshComponent, InHitResult.FaceIndex);

		const auto& Map_ = GetView()->GetMap();

		if (Map_)
		{
			auto layer = MakeShared<Esri::GameEngine::Layers::Base::ArcGISLayer>(
				Esri::GameEngine::Map::ArcGISMapInternal::FindLayerById(Map_->APIObject.Get(), LayerId));
			output.Layer = NewObject<UArcGISLayer>();
			output.Layer->APIObject = layer;
		}
	}

	return output;
}

bool UArcGISMapComponent::GetAreMeshCollidersEnabled() const
{
	return bAreMeshCollidersEnabled;
}

void UArcGISMapComponent::ApplyAPIKeyUpdate()
{
	InvalidateLayers();

	InitializeMap();

	SetMapToView();
}

void UArcGISMapComponent::ApplyBasemapUpdate()
{
	UArcGISBasemap* arcGISBasemap{nullptr};

	if (Basemap != "")
	{
		if (BasemapType != EBaseMapTypes::ImageLayer)
		{
			arcGISBasemap = UArcGISBasemap::CreateArcGISBasemapWithBasemapSource(Basemap, APIKey);
		}
		else
		{
			arcGISBasemap = UArcGISBasemap::CreateArcGISBasemapWithLayerSourceAndType(Basemap, EArcGISLayerType::Type::ArcGISImageLayer, APIKey);
		}
	}

	Map->SetBasemap(arcGISBasemap);

	if (Map->APIObject->GetLoadStatus() == Esri::GameEngine::ArcGISLoadStatus::FailedToLoad)
	{
		Map->APIObject->RetryLoad();
	}
}

void UArcGISMapComponent::ApplyEditorModeUpdate()
{
	if (GetWorld()->WorldType != EWorldType::Editor)
	{
		return;
	}

	if (!bIsEditorModeEnabled)
	{
		DestroyView();
	}

	OnEditorModeChanged.Broadcast();

	SetMapToView();
}

void UArcGISMapComponent::ApplyElevationUpdate()
{
	UArcGISMapElevation* arcGISMapElevation{nullptr};

	if (EnableElevation && ElevationSource != "")
	{
		auto elevationLayer = UArcGISImageElevationSource::CreateArcGISImageElevationSourceWithName(ElevationSource, "Elevation", APIKey);
		elevationLayer->SetIsEnabled(EnableElevation);
		arcGISMapElevation = UArcGISMapElevation::CreateArcGISMapElevationWithElevationSource(elevationLayer);
	}

	Map->SetElevation(arcGISMapElevation);
}

void UArcGISMapComponent::ApplyExtentUpdate()
{
	// SetClippingArea should not be called when the map type is global
	if (GetMapType() == EArcGISMapType::Local)
	{
		if (EnableExtent && IsExtentDefinitionValid(Extent))
		{
			Map->SetClippingArea(CreateArcGISExtentFromDefinition(Extent));
		}
		else
		{
			Map->SetClippingArea(nullptr);
		}
	}
}

#if WITH_EDITOR
void UArcGISMapComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = PropertyChangedEvent.MemberProperty->GetFName();

	if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, bAreMeshCollidersEnabled))
	{
		SetAreMeshCollidersEnabled(bAreMeshCollidersEnabled);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, bIsEditorModeEnabled))
	{
		ApplyEditorModeUpdate();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, bUpdateGeoReferencingSystem))
	{
		PushToGeoReferencingSystem();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, APIKey))
	{
		ApplyAPIKeyUpdate();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, AuthenticationConfigurations))
	{
		const int32 ArrayIndex = PropertyChangedEvent.GetArrayIndex(PropertyName.ToString());

		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
		{
			if (AuthenticationConfigurations.IsValidIndex(ArrayIndex))
			{
				AuthenticationConfigurations[ArrayIndex].ID = FAuthGuid::NewGuid();
				AuthenticationConfigurations[ArrayIndex].Name = TEXT("Configuration ") + FString::FromInt(ArrayIndex);
			}
		}
		else if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Duplicate)
		{
			if (Layers.IsValidIndex(ArrayIndex))
			{
				AuthenticationConfigurations[ArrayIndex].ID = FAuthGuid::NewGuid();
				AuthenticationConfigurations[ArrayIndex].Name = TEXT("Configuration ") + FString::FromInt(ArrayIndex) + TEXT(" Duplicate");
			}
		}

		UpdateAuthenticationConfigurations();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, Basemap) ||
			 PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, BasemapType))
	{
		ApplyBasemapUpdate();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, EnableElevation) ||
			 PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, ElevationSource))
	{
		ApplyElevationUpdate();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, EnableExtent) ||
			 PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, Extent))
	{
		ApplyExtentUpdate();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, Layers))
	{
		const int32 ArrayIndex = PropertyChangedEvent.GetArrayIndex(PropertyName.ToString());

		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ArrayAdd)
		{
			if (Layers.IsValidIndex(ArrayIndex))
			{
				Layers[ArrayIndex].ID = FGuid::NewGuid();
				Layers[ArrayIndex].Name = TEXT("New Layer");
			}
		}
		else if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Duplicate)
		{
			if (Layers.IsValidIndex(ArrayIndex))
			{
				Layers[ArrayIndex].ID = FGuid::NewGuid();
				Layers[ArrayIndex].Name = Layers[ArrayIndex].Name + TEXT(" Duplicate");
				Layers[ArrayIndex].APIObject.Reset();
			}
		}

		UpdateLayers();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, MapType))
	{
		ApplyMapTypeUpdate();
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, OriginPosition) ||
			 PropertyName == GET_MEMBER_NAME_CHECKED(UArcGISMapComponent, bIsOriginAtPlanetCenter))
	{
		UpdateENUToWorldTransform();
		PushToGeoReferencingSystem();

		OnOriginPositionChanged.Broadcast();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UArcGISMapComponent::OnGeoReferencingSystemChanged(AGeoReferencingSystem* InGeoReferencingSystem)
{
	GeoReferencingSystem = InGeoReferencingSystem;

	PushToGeoReferencingSystem();
}

void UArcGISMapComponent::OnRegister()
{
	Super::OnRegister();

	Initialize();

	if (!Map)
	{
		ResetMap();
	}

	SetMapToView();
}

void UArcGISMapComponent::ResetMap()
{
	InvalidateLayers();

	InitializeMap();
}

void UArcGISMapComponent::InitializeMap()
{
	Map = TStrongObjectPtr<UArcGISMap>(UArcGISMap::CreateArcGISMapWithMapType(MapType));

	ApplyBasemapUpdate();

	ApplyElevationUpdate();

	ApplyExtentUpdate();

	UpdateLayers();

	UpdateAuthenticationConfigurations();
}

void UArcGISMapComponent::InitializeMemorySystem()
{
	GetMemorySystemHandler()->Initialize(this);

	SetMemoryQuotas(GetMemorySystemHandler()->GetMemoryQuotas());
}

void UArcGISMapComponent::InvalidateLayers()
{
	LastLayers.Reset(0);

	for (int32 i = 0; i < Layers.Num(); i++)
	{
		Layers[i].APIObject.Reset();
	}

	if (Map)
	{
		Map->GetLayers()->RemoveAll();
	}
}

void UArcGISMapComponent::NotifyLowMemoryWarning()
{
	GetView()->HandleLowMemoryWarning();
}

void UArcGISMapComponent::UpdateLayers()
{
	bool hasChanges = false;

	for (int32 i = 0; i < Layers.Num(); i++)
	{
		auto& layer = Layers[i];

		if (!layer.ID.IsValid())
		{
			layer.ID = FGuid::NewGuid();

			hasChanges = true;
		}

		auto lastIndex = LastLayers.Find(layer);

		if (lastIndex > -1)
		{
			const auto& lastLayer = LastLayers[lastIndex];

			// Reset the API Object if data is not valid anymore or the source changed
			if (!CanCreateArcGISLayerFromDefinition(layer) || lastLayer.Source != layer.Source || lastLayer.Type != layer.Type)
			{
				layer.APIObject.Reset();

				hasChanges = true;
			}

			if (layer.APIObject.IsValid() && lastLayer.Opacity != layer.Opacity)
			{
				layer.Opacity = FMath::Clamp<float>(layer.Opacity, 0.0f, 1.0f);

				layer.APIObject->SetOpacity(layer.Opacity);

				hasChanges = true;
			}

			if (layer.APIObject.IsValid() && lastLayer.Name != layer.Name)
			{
				layer.APIObject->SetName(layer.Name);

				hasChanges = true;
			}

			if (layer.APIObject.IsValid() && lastLayer.bIsVisible != layer.bIsVisible)
			{
				layer.APIObject->SetIsVisible(layer.bIsVisible);

				hasChanges = true;
			}
		}
	}

	auto CurrentLayers = LastLayers;

	// Remove from the API the ones either removed from the UI or not valid anymore
	for (int32 i = 0; i < LastLayers.Num(); i++)
	{
		const auto& layer = LastLayers[i];

		auto currentIndex = Layers.Find(layer);

		if (currentIndex < 0 || !Layers[currentIndex].APIObject.IsValid())
		{
			auto lastIndex = CurrentLayers.Find(layer);

			Map->GetLayers()->Remove(lastIndex);
			CurrentLayers.RemoveAt(lastIndex);

			hasChanges = true;
		}
	}

	LastLayers.Empty();

	// Add to the API the ones either new in the UI or now valid
	// Also reorder layers in the API if necessary
	for (int32 i = 0, j = 0; i < Layers.Num(); i++)
	{
		auto& layer = Layers[i];

		if (!layer.APIObject.IsValid())
		{
			layer.APIObject = TStrongObjectPtr<UArcGISLayer>(CreateArcGISLayerFromDefinition(layer, APIKey));

			if (layer.APIObject.IsValid())
			{
				Map->GetLayers()->Insert(j, layer.APIObject.Get());
				CurrentLayers.Insert(layer, j);

				hasChanges = true;
			}
		}
		else if (auto lastIndex = CurrentLayers.Find(layer); lastIndex != j)
		{
			Map->GetLayers()->Move(lastIndex, j);
			CurrentLayers.Swap(lastIndex, j);

			hasChanges = true;
		}

		if (layer.APIObject.IsValid())
		{
			LastLayers.Add(layer);

			++j;
		}
	}

	if (hasChanges)
	{
		MarkPackageDirty();
	}

	if (Map->APIObject->GetLoadStatus() == Esri::GameEngine::ArcGISLoadStatus::FailedToLoad)
	{
		Map->APIObject->RetryLoad();
	}
}

void UArcGISMapComponent::UpdateLayerGuid(FArcGISLayerInstanceData* InUpdatedLayer)
{
	for (auto& layer : Layers)
	{
		if (layer == *InUpdatedLayer)
		{
			layer.AuthConfigID = InUpdatedLayer->AuthConfigID;
			return;
		}
	}
}

void UArcGISMapComponent::BeginDestroy()
{
	DestroyView();

	Super::BeginDestroy();
}

void UArcGISMapComponent::DestroyView()
{
	if (View.IsValid())
	{
		View->SetMap(nullptr);

		View->ConditionalBeginDestroy();

		View.Reset();
	}
}

FGeoPosition UArcGISMapComponent::EngineToGeographic(const FVector3d& InEngineCoordinates)
{
	if (!HasSpatialReference())
	{
		UE_LOG(LogTemp, Warning, TEXT("Default Position. No Spatial Reference"));
		return FGeoPosition();
	}

	const auto& ViewAPI = GetView()->APIObject;
	{
		auto CartesianPosition = FromEnginePosition(InEngineCoordinates);
		auto GeographicPosition = ViewAPI->WorldToGeographic(CartesianPosition);
		return FGeoPosition(GeographicPosition.GetX(), GeographicPosition.GetY(), GeographicPosition.GetZ(),
							GeographicPosition.GetSpatialReference());
	}
}

FVector3d UArcGISMapComponent::FromEnginePosition(const FVector3d& InEnginePosition)
{
	// Consider UE's world offset
	FIntVector WorldOriginOffset = GetWorld()->OriginLocation;
	FVector3d UERebasedCoordinates = InEnginePosition + FVector3d(WorldOriginOffset.X, WorldOriginOffset.Y, WorldOriginOffset.Z);

	// Convert UE units to meters and flip the Y axis to match the GeoReferencing plugin convention
	FVector3d ENUFrameCoordinates = FromUnrealCoordSystem.TransformPosition(UERebasedCoordinates);

	return ENUFrameToWorldFrame.TransformPosition(ENUFrameCoordinates);
}

FVector3d UArcGISMapComponent::GeographicToEngine(const FGeoPosition& InGeographicCoordinates)
{
	if (!HasSpatialReference())
	{
		UE_LOG(LogTemp, Warning, TEXT("Default Position. No Spatial Reference"));
		return FVector3d();
	}

	const auto& ViewAPI = GetView()->APIObject;
	{
		return ToEnginePosition(ViewAPI->GeographicToWorld(static_cast<Esri::GameEngine::Geometry::ArcGISPoint>(InGeographicCoordinates)));
	}
}

bool UArcGISMapComponent::InsertAuthenticationConfig(const FGuid& InAuthConfigId, const FString& InSource)
{
	if (!InAuthConfigId.IsValid())
	{
		return false;
	}

	for (const auto& authenticationConfigurationData : AuthenticationConfigurations)
	{
		if (InAuthConfigId != authenticationConfigurationData.ID)
		{
			continue;
		}

		auto trimmedURI = InSource.TrimStartAndEnd();
		auto trimmedClientID = authenticationConfigurationData.ClientID.TrimStartAndEnd();
		auto trimmedClientSecret = authenticationConfigurationData.ClientSecret.TrimStartAndEnd();
		auto trimmedRedirectURI = authenticationConfigurationData.RedirectURI.TrimStartAndEnd();

		if (trimmedURI.IsEmpty() || trimmedClientID.IsEmpty() || (trimmedClientSecret.IsEmpty() && trimmedRedirectURI.IsEmpty()))
		{
			continue;
		}

		auto authenticationConfiguration = Esri::GameEngine::Security::ArcGISOAuthAuthenticationConfiguration(
			TCHAR_TO_ANSI(*trimmedClientID), TCHAR_TO_ANSI(*trimmedClientSecret), TCHAR_TO_ANSI(*trimmedRedirectURI));

		Esri::GameEngine::Security::ArcGISAuthenticationManager::GetAuthenticationConfigurations().Insert(TCHAR_TO_ANSI(*trimmedURI),
																										  authenticationConfiguration);

		return true;
	}

	return false;
}

void UArcGISMapComponent::UpdateAuthenticationConfigurations()
{
	if (AuthenticationConfigurations.IsEmpty())
	{
		OnAuthenticationChanged.Broadcast();
		return;
	}

	for (auto& authenticationConfiguration : AuthenticationConfigurations)
	{
		if (!authenticationConfiguration.ID.IsValid())
		{
			authenticationConfiguration.ID = FAuthGuid::NewGuid();
		}
	}

	Esri::GameEngine::Security::ArcGISAuthenticationManager::GetAuthenticationConfigurations().RemoveAll();

	for (const auto& layer : Layers)
	{
		if (InsertAuthenticationConfig(layer.AuthConfigID, layer.Source))
		{
			layer.APIObject->APIObject->RetryLoad();
		}
	}

	if (InsertAuthenticationConfig(BasemapAuthConfigID, Basemap))
	{
		Map->GetBasemap()->APIObject->RetryLoad();
	}

	if (InsertAuthenticationConfig(ElevationAuthConfigID, ElevationSource))
	{
		auto elevationSources = Map->GetElevation()->APIObject->GetElevationSources();

		for (auto elevationSource : elevationSources)
		{
			elevationSource.RetryLoad();
		}
	}

	OnAuthenticationChanged.Broadcast();
}

bool UArcGISMapComponent::ZoomToExtent(AActor* InActor, UArcGISExtent* InExtent)
{
	if (!HasSpatialReference())
	{
		UE_LOG(LogTemp, Warning, TEXT("View must have a spatial reference to run zoom to layer"));
		return false;
	}

	auto spatialReference = GetMap()->GetSpatialReference();

	if (spatialReference == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("View must have a spatial reference to run zoom to layer"));
		return false;
	}

	if (InExtent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Extent cannot be nullptr"));
		return false;
	}

	auto cameraPosition = InExtent->GetCenter();
	double largeSide = 0;

	auto rectangleExtent = static_cast<UArcGISExtentRectangle*>(InExtent);
	auto circleExtent = static_cast<UArcGISExtentCircle*>(InExtent);
	if (rectangleExtent != nullptr)
	{
		largeSide = FMath::Max(rectangleExtent->GetWidth(), rectangleExtent->GetHeight());
	}
	else if (circleExtent != nullptr)
	{
		largeSide = circleExtent->GetRadius() * 2;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Extent type is not recognized"));
		return false;
	}

	if (MapType == EArcGISMapType::Global)
	{
		auto globeRadius = spatialReference->GetSpheroidData().MajorSemiAxis;
		largeSide = FMath::Min(largeSide, 2 * globeRadius);
	}

	auto Controller = GetWorld()->GetFirstPlayerController();

	double radFOVAngle = FMath::DegreesToRadians(Controller ? Controller->PlayerCameraManager->GetFOVAngle() : 90.f);
	double radHFOV = FMath::Atan(FMath::Tan(radFOVAngle / 2));
	double zOffset = 0.5 * largeSide / FMath::Tan(radHFOV);

	auto newPosition = UArcGISPoint::CreateArcGISPointWithXYZSpatialReference(
		cameraPosition->GetX(), cameraPosition->GetY(), cameraPosition->GetZ() + zOffset, cameraPosition->GetSpatialReference());

	auto locationComponent = Cast<UArcGISLocationComponent>(InActor->GetComponentByClass(UArcGISLocationComponent::StaticClass()));
	if (locationComponent)
	{
		locationComponent->SetPosition(newPosition);
		locationComponent->SetRotation(UArcGISRotation::CreateArcGISRotation(0, 0, 0));

		return true;
	}

	auto position = GeographicToEngine(newPosition);
	FRotator rotation{0, 0, 0};
	InActor->SetActorLocationAndRotation(position, rotation);
	return true;
}

bool UArcGISMapComponent::ZoomToLayer(AActor* InActor, FArcGISLayerInstanceData InLayer)
{
	if (InLayer.APIObject.IsValid())
	{
		auto task = ZoomToLayerAsync(InActor, InLayer.APIObject.Get());
		return task.GetResult();
	}

	return false;
}

UE::Tasks::TTask<bool> UArcGISMapComponent::ZoomToLayerAsync(AActor* InActor, UArcGISLayer* InLayer)
{
	if (InLayer == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid layer passed to zoom to layer"));
		return UE::Tasks::Launch(UE_SOURCE_LOCATION, [] {
			return false;
		});
	}

	if (InLayer->APIObject->GetLoadStatus() != Esri::GameEngine::ArcGISLoadStatus::Loaded)
	{
		if (InLayer->APIObject->GetLoadStatus() == Esri::GameEngine::ArcGISLoadStatus::NotLoaded)
		{
			InLayer->APIObject->Load();
		}
		else if (InLayer->APIObject->GetLoadStatus() == Esri::GameEngine::ArcGISLoadStatus::FailedToLoad)
		{
			InLayer->APIObject->RetryLoad();
		}

		return UE::Tasks::Launch(UE_SOURCE_LOCATION, [this, InActor, InLayer] {
			while (InLayer->APIObject->GetLoadStatus() == Esri::GameEngine::ArcGISLoadStatus::Loading)
			{
			}

			if (InLayer->APIObject->GetLoadStatus() == Esri::GameEngine::ArcGISLoadStatus::FailedToLoad)
			{
				UE_LOG(LogTemp, Warning, TEXT("Layer passed to zoom to layer must be loaded"));
				return false;
			}

			auto extent = InLayer->GetExtent();

			if (extent == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("The layer passed to zoom to layer does not have a valid extent"));
				return false;
			}

			return ZoomToExtent(InActor, extent);
		});
	}

	return UE::Tasks::Launch(UE_SOURCE_LOCATION, [this, InActor, InLayer] {
		return ZoomToExtent(InActor, InLayer->GetExtent());
	});
}
