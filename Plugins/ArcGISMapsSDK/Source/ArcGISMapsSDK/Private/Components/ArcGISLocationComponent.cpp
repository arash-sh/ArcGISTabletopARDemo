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
#include "ArcGISMapsSDK/Components/ArcGISLocationComponent.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISGeometryEngine.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

UArcGISLocationComponent::UArcGISLocationComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;

	bTickInEditor = true;
	bAutoActivate = true;

	bWantsOnUpdateTransform = true;
	bInternalHasChanged = true;
}

void UArcGISLocationComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateTransform();
}

void UArcGISLocationComponent::OnComponentCreated()
{
	bNeedsInitialization = true;
}

void UArcGISLocationComponent::OnUnregister()
{
	if (MapComponent.IsValid())
	{
		MapComponent->OnSpatialReferenceChanged.RemoveAll(this);
	}

	Super::OnUnregister();
}

void UArcGISLocationComponent::OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent)
{
	if (MapComponent.IsValid() && MapComponent != InMapComponent)
	{
		MapComponent->OnSpatialReferenceChanged.RemoveAll(this);
	}

	if (MapComponent != InMapComponent)
	{
		MapComponent = InMapComponent;

		if (bNeedsInitialization)
		{
			UpdateGeoPositionAndRotation();
		}
	}

	if (MapComponent.IsValid())
	{
		// OnSpatialReferenceChanged gets serialized so we should make sure a unique binding exists
		if (!MapComponent->OnSpatialReferenceChanged.Contains(this, GET_FUNCTION_NAME_CHECKED(UArcGISLocationComponent, UpdateTransform)))
		{
			MapComponent->OnSpatialReferenceChanged.AddUniqueDynamic(this, &UArcGISLocationComponent::UpdateTransform);
		}

		if (!MapComponent->OnOriginPositionChanged.Contains(this, GET_FUNCTION_NAME_CHECKED(UArcGISLocationComponent, UpdateTransform)))
		{
			MapComponent->OnOriginPositionChanged.AddUniqueDynamic(this, &UArcGISLocationComponent::UpdateTransform);
		}
	}
}

UArcGISPoint* UArcGISLocationComponent::GetPosition()
{
	return Position;
}

UArcGISRotation* UArcGISLocationComponent::GetRotation()
{
	return UArcGISRotation::CreateArcGISRotation(Rotation.Pitch, Rotation.Roll, Rotation.Heading);
}

void UArcGISLocationComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	if (!bTransformHasChanged)
	{
		UpdateGeoPositionAndRotation();
	}

	bTransformHasChanged = false;
}

#if WITH_EDITORONLY_DATA
void UArcGISLocationComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateTransform();
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UArcGISLocationComponent::SetPosition(UArcGISPoint* InPosition)
{
	Position = InPosition;

	bInternalHasChanged = true;
}

void UArcGISLocationComponent::SetRotation(UArcGISRotation* InRotation)
{
	Rotation.Pitch = InRotation->GetPitch();
	Rotation.Heading = InRotation->GetHeading();
	Rotation.Roll = InRotation->GetRoll();

	bInternalHasChanged = true;
}

void UArcGISLocationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bInternalHasChanged)
	{
		UpdateTransform();
	}
}

void UArcGISLocationComponent::UpdateTransform()
{
	if (MapComponent.IsValid() && MapComponent->ShouldEditorComponentBeUpdated() && MapComponent->HasSpatialReference())
	{
		auto EngineLocation = MapComponent->GeographicToEngine(Position);

		if (FMath::IsNaN(EngineLocation.X) || FMath::IsNaN(EngineLocation.Y) || FMath::IsNaN(EngineLocation.Z))
		{
			return;
		}

		// Get a transform to go from a ENU based at the object location to the View ENU
		// This is required because the API rotation is stored relative to the object ENU
		auto ObjectENUToViewENU = MapComponent->GetENUAtLocationToViewENUTransform(EngineLocation);

		// The API euler angles need to be converted to the Unreal convention
		const auto GeographicRotator = FRotator(Rotation.Pitch - 90, Rotation.Heading - 90, -Rotation.Roll);

		// Get the rotation relative to the View ENU
		const auto NewCameraRotation = ObjectENUToViewENU.GetRotation() * GeographicRotator.Quaternion();

		bInternalHasChanged = false;
		bTransformHasChanged = true;

		GetOwner()->SetActorLocationAndRotation(EngineLocation, NewCameraRotation.Rotator(), false, nullptr, ETeleportType::ResetPhysics);
	}
}

void UArcGISLocationComponent::UpdateGeoPositionAndRotation()
{
	if (MapComponent.IsValid() && MapComponent->HasSpatialReference())
	{
		auto EngineLocation = GetOwner()->GetActorLocation();
		auto GeographicCoordinates = MapComponent->EngineToGeographic(EngineLocation);

		if (!Position.IsValid())
		{
			return;
		}

		if (Position.SpatialReferenceWkid != GeographicCoordinates.SpatialReferenceWkid)
		{
			auto toSR = UArcGISSpatialReference::CreateArcGISSpatialReference(Position.SpatialReferenceWkid);
			auto geometry = UArcGISGeometryEngine::Project(GeographicCoordinates, toSR);
			if (geometry != nullptr)
			{
				GeographicCoordinates = static_cast<UArcGISPoint*>(geometry);
			}
		}

		Position = GeographicCoordinates;

		// Get a transform to go from the View ENU to a ENU based at the object location
		// This is required because the API rotation is stored relative to the object ENU
		auto ViewENUToObjectENU = MapComponent->GetENUAtLocationToViewENUTransform(EngineLocation).Inverse();

		// Get the rotation relative to the object ENU
		const auto GeographicQuaternion = ViewENUToObjectENU.GetRotation() * GetComponentRotation().Quaternion();

		const auto GeographicRotator = GeographicQuaternion.Rotator();

		// The Unreal euler angles need to be converted to the API convention
		Rotation.Pitch = GeographicRotator.Pitch + 90;
		Rotation.Heading = GeographicRotator.Yaw + 90;
		Rotation.Roll = -GeographicRotator.Roll;
	}
}
