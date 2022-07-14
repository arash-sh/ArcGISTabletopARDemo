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
#include "ArcGISMapsSDK/Components/ArcGISSkyAtmosphereRepositionComponent.h"

#if WITH_EDITOR
#include "EditorViewportClient.h"
#endif
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

#include "ArcGISMapsSDK/Utils/Unreal/AtmosphereHelpers.h"
#include "ArcGISMapsSDK/Utils/Unreal/CameraHelpers.h"

UArcGISSkyAtmosphereRepositionComponent::UArcGISSkyAtmosphereRepositionComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_LastDemotable;

	bTickInEditor = true;
	bAutoActivate = true;
}

void UArcGISSkyAtmosphereRepositionComponent::BeginPlay()
{
	Super::BeginPlay();

	LastCameraPosition = FVector3d(0, 0, 0);
	bInitialized = false;
}

void UArcGISSkyAtmosphereRepositionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UArcGISSkyAtmosphereRepositionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bInitialized)
	{
		Initialize();
	}

	if (MapComponent.IsValid() && MapComponent->ShouldEditorComponentBeUpdated() && MapComponent->HasSpatialReference())
	{
		auto CameraPosition = UCameraHelpers::GetActiveCameraPosition(Cast<AArcGISMapActor>(MapComponent.Get()->GetOwner()));

		if (LastCameraPosition != CameraPosition)
		{
			UpdateSkyAtmospherePosition(CameraPosition);
			UpdateAtmosphereBlending(CameraPosition);

			LastCameraPosition = CameraPosition;
		}
	}
}

void UArcGISSkyAtmosphereRepositionComponent::OnUnregister()
{
	if (MapComponent.IsValid())
	{
		MapComponent->OnMapTypeChanged.RemoveAll(this);
		MapComponent->OnSpatialReferenceChanged.RemoveAll(this);
	}

	Super::OnUnregister();
}

void UArcGISSkyAtmosphereRepositionComponent::OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent)
{
	if (MapComponent.IsValid() && MapComponent != InMapComponent)
	{
		MapComponent->OnMapTypeChanged.RemoveAll(this);
		MapComponent->OnSpatialReferenceChanged.RemoveAll(this);
	}

	if (MapComponent != InMapComponent)
	{
		MapComponent = InMapComponent;
	}

	if (MapComponent.IsValid())
	{
		// OnSpatialReferenceChanged and OnMapTypeChanged get serialized so we should make sure a unique binding exists
		if (!MapComponent->OnSpatialReferenceChanged.Contains(
				this, GET_FUNCTION_NAME_CHECKED(UArcGISSkyAtmosphereRepositionComponent, UpdateSkyAtmospherePositionAndBlending)))
		{
			MapComponent->OnSpatialReferenceChanged.AddUniqueDynamic(
				this, &UArcGISSkyAtmosphereRepositionComponent::UpdateSkyAtmospherePositionAndBlending);
		}

		if (!MapComponent->OnMapTypeChanged.Contains(
				this, GET_FUNCTION_NAME_CHECKED(UArcGISSkyAtmosphereRepositionComponent, UpdateSkyAtmospherePositionAndBlending)))
		{
			MapComponent->OnMapTypeChanged.AddUniqueDynamic(this, &UArcGISSkyAtmosphereRepositionComponent::UpdateSkyAtmospherePositionAndBlending);
		}
	}
}

void UArcGISSkyAtmosphereRepositionComponent::Initialize()
{
	auto SkyAtmosphere = Cast<ASkyAtmosphere>(UGameplayStatics::GetActorOfClass(this, ASkyAtmosphere::StaticClass()));

	if (SkyAtmosphere)
	{
		SkyAtmosphereComponent = SkyAtmosphere->GetComponent();
	}

	bInitialized = true;
}

void UArcGISSkyAtmosphereRepositionComponent::UpdateSkyAtmospherePositionAndBlending()
{
	if (MapComponent.IsValid())
	{
		auto CameraPosition = UCameraHelpers::GetActiveCameraPosition(Cast<AArcGISMapActor>(MapComponent.Get()->GetOwner()));

		UpdateSkyAtmospherePosition(CameraPosition);
		UpdateAtmosphereBlending(CameraPosition);

		LastCameraPosition = CameraPosition;
	}
}

void UArcGISSkyAtmosphereRepositionComponent::UpdateSkyAtmospherePosition(const FVector3d& CameraPosition)
{
	FVector3d NewPosition(0, 0, 0);

	auto MapType = MapComponent->GetMapType();

	if (MapType == EArcGISMapType::Global)
	{
		NewPosition = UAtmosphereHelpers::GetGlobalViewSkyAtmospherePositionFrom(MapComponent.Get(), CameraPosition);
	}
	else
	{
		NewPosition = UAtmosphereHelpers::GetLocalViewSkyAtmospherePositionFrom(CameraPosition);
	}

	GetOwner()->SetActorLocation(NewPosition, false, nullptr, ETeleportType::ResetPhysics);
}

void UArcGISSkyAtmosphereRepositionComponent::UpdateAtmosphereBlending(const FVector3d& CameraPosition)
{
	if (SkyAtmosphereComponent)
	{
		float NewDistanceScale = UAtmosphereHelpers::GetAerialPerspectiveViewDistanceScaleFrom(
			MapComponent.Get(), CameraPosition, SkyAtmosphereComponent->AtmosphereHeight, SkyAtmosphereComponent->AerialPespectiveViewDistanceScale,
			50000.0f);
		// TODO: Fix this
		//SkyAtmosphereComponent->SetAerialPespectiveViewDistanceScale(NewDistanceScale);
	}
}
