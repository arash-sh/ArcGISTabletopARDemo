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
#include "ArcGISMapsSDK/Components/ArcGISCameraComponent.h"

#if WITH_EDITOR
#include "EditorViewportClient.h"
#endif
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/MapView/ArcGISCamera.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Subsystem/ArcGISMapsSDKSubsystem.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoUtils.h"

UArcGISCameraComponent::UArcGISCameraComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;

	bTickInEditor = true;
	bAutoActivate = true;

	// The update of the camera has to be done at the end of the frame, when all the actors and pawn have their position defined.
	// If we changed the camera during the update, some actors could use the new transformation and some the old one, resulting in objects popping in/out one frame.
	PrimaryComponentTick.TickGroup = TG_LastDemotable;
}

void UArcGISCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	PushViewportProperties();
}

UArcGISCameraComponent::FProperties UArcGISCameraComponent::CalculateProperties()
{
	FProperties properties;

	properties.HorizontalFov = FieldOfView;

#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		FViewport* ActiveViewport = GEditor->GetActiveViewport();
		FEditorViewportClient* EditorViewClient =
			(ActiveViewport != nullptr) ? static_cast<FEditorViewportClient*>(ActiveViewport->GetClient()) : nullptr;
		if (EditorViewClient)
		{
			FIntPoint Origin, Size;
			EditorViewClient->GetViewportDimensions(Origin, Size);
			properties.ViewportSizeX = Size.X;
			properties.ViewportSizeY = Size.Y;

			properties.HorizontalFov = EditorViewClient->ViewFOV;
		}
	}
	else
#endif
	{
		if (auto Controller = GetWorld()->GetFirstPlayerController())
		{
			Controller->GetViewportSize(properties.ViewportSizeX, properties.ViewportSizeY);
			properties.HorizontalFov = Controller->PlayerCameraManager->GetFOVAngle();
		}
	}

	return properties;
}

UArcGISCameraComponent::FTransform UArcGISCameraComponent::CalculateTransform()
{
	FTransform transform;

#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		if (auto activeViewport = GEditor->GetActiveViewport())
		{
			if (auto editorViewClient = static_cast<FEditorViewportClient*>(activeViewport->GetClient()))
			{
				transform.Position = editorViewClient->GetViewLocation();
				transform.Rotation = editorViewClient->GetViewRotation();

				// UE5 uses Rotator(0, 90, 0) as a base when using a orbit camera. We have to transform Yaw to have the same result.
				if (editorViewClient->bUsingOrbitCamera)
				{
					transform.Rotation.Yaw = 90 - transform.Rotation.Yaw;
				}
			}
		}
	}
	else
#endif
	{
		transform.Position = GetOwner()->GetActorLocation();
		transform.Rotation = GetOwner()->GetActorRotation();
	}

	return transform;
}

void UArcGISCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (UWorld* OwningWorld = GetWorld(); OwningWorld && OwningWorld->WorldType != EWorldType::Inactive)
	{
		if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
		{
			arcGISMapsSDKSubsystem->RegisterListener(this);
		}
	}
}

void UArcGISCameraComponent::OnUnregister()
{
	if (MapComponent)
	{
		MapComponent->OnSpatialReferenceChanged.RemoveAll(this);
	}

	if (UWorld* OwningWorld = GetWorld(); OwningWorld && OwningWorld->WorldType != EWorldType::Inactive)
	{
		if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
		{
			arcGISMapsSDKSubsystem->UnregisterListener(this);
		}
	}

	Super::OnUnregister();
}

void UArcGISCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MapComponent && MapComponent->ShouldEditorComponentBeUpdated())
	{
		PushViewportProperties();

		PushPosition();
	}
}

void UArcGISCameraComponent::OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent)
{
	if (MapComponent && MapComponent != InMapComponent)
	{
		MapComponent->OnEditorModeChanged.RemoveAll(this);
		MapComponent->OnSpatialReferenceChanged.RemoveAll(this);
	}

	if (MapComponent != InMapComponent)
	{
		MapComponent = InMapComponent;

		Properties.HorizontalFov = 0;
		Properties.ViewportSizeX = 0;
		Properties.ViewportSizeY = 0;

		PushViewportProperties();
	}

	if (MapComponent)
	{
		// OnEditorModeChanged and OnSpatialReferenceChanged get serialized so we should make sure a unique binding exists
		if (!MapComponent->OnEditorModeChanged.Contains(this, GET_FUNCTION_NAME_CHECKED(UArcGISCameraComponent, OnEditorModeChanged)))
		{
			MapComponent->OnEditorModeChanged.AddUniqueDynamic(this, &UArcGISCameraComponent::OnEditorModeChanged);
		}

		if (!MapComponent->OnSpatialReferenceChanged.Contains(this, GET_FUNCTION_NAME_CHECKED(UArcGISCameraComponent, PushPosition)))
		{
			MapComponent->OnSpatialReferenceChanged.AddUniqueDynamic(this, &UArcGISCameraComponent::PushPosition);
		}
	}
}

void UArcGISCameraComponent::OnEditorModeChanged()
{
	if (MapComponent && MapComponent->ShouldEditorComponentBeUpdated())
	{
		Properties.HorizontalFov = 0;
		Properties.ViewportSizeX = 0;
		Properties.ViewportSizeY = 0;
	}
}

void UArcGISCameraComponent::PushPosition()
{
	if (MapComponent && MapComponent->HasSpatialReference())
	{
		auto transform = CalculateTransform();

		auto GeographicCoordinates = MapComponent->EngineToGeographic(transform.Position);

		if (!GeographicCoordinates.IsValid())
		{
			return;
		}

		// Get a transform to go from the View ENU to a ENU based at the camera location
		// This is required because the API expects the rotation to be relative to the camera ENU
		const auto ViewENUToCameraENU = MapComponent->GetENUAtLocationToViewENUTransform(transform.Position).Inverse();

		// Get the rotation relative to the camera ENU
		const auto NewRotation = ViewENUToCameraENU.GetRotation() * transform.Rotation.Quaternion();

		// Note that ENU and Unreal cameras are both left handed, which permits this direct calculation of the rotator without having to transform axis.
		const auto Rotator = NewRotation.Rotator();

		auto ArcGISCamera =
			UArcGISCamera::CreateArcGISCameraWithLocationHeadingPitchRoll(GeographicCoordinates, Rotator.Yaw + 90, Rotator.Pitch + 90, -Rotator.Roll);

		MapComponent->GetView()->SetCamera(ArcGISCamera);
	}
}

void UArcGISCameraComponent::PushViewportProperties()
{
	if (!MapComponent)
	{
		return;
	}

	auto newProperties = CalculateProperties();

	if (Properties.HorizontalFov != newProperties.HorizontalFov || Properties.ViewportSizeX != newProperties.ViewportSizeX ||
		Properties.ViewportSizeY != newProperties.ViewportSizeY)
	{
		Properties.HorizontalFov = newProperties.HorizontalFov;
		Properties.ViewportSizeX = newProperties.ViewportSizeX;
		Properties.ViewportSizeY = newProperties.ViewportSizeY;

		if (Properties.ViewportSizeX != 0)
		{
			float HFovRadians = FMath::DegreesToRadians(Properties.HorizontalFov);

			// Comes from aspect_ratio = width / height = tan (hfov / 2) / tan (vfov / 2)
			float VFovRadians =
				2.0f * FMath::Atan(FMath::Tan(HFovRadians / 2.0f) * (Properties.ViewportSizeY / static_cast<float>(Properties.ViewportSizeX)));

			float VerticalFov = FMath::RadiansToDegrees(VFovRadians);

			MapComponent->SetViewportProperties(Properties.ViewportSizeX, Properties.ViewportSizeY, Properties.HorizontalFov, VerticalFov, 1);
		}
	}
}
