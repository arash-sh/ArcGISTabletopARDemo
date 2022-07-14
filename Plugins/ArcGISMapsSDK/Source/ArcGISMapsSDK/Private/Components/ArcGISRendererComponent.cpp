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
#include "ArcGISMapsSDK/Components/ArcGISRendererComponent.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Renderer/ArcGISRenderer.h"

UArcGISRendererComponent::UArcGISRendererComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = true;

	bTickInEditor = true;
	bAutoActivate = true;

	// The renderer has to be updated at the beginning of the frame, before any other component, so the reposition is done at the beginning too.
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UArcGISRendererComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ArcGISRenderer.Reset();

	Super::EndPlay(EndPlayReason);
}

void UArcGISRendererComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (MapComponent.IsValid() && MapComponent->ShouldEditorComponentBeUpdated())
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		if (ArcGISRenderer)
		{
			ArcGISRenderer->Update();
		}
	}
}

uint32 UArcGISRendererComponent::GetLayerIdByMeshComponent(const UMeshComponent* InMeshComponent) const
{
	return ArcGISRenderer->GetLayerIdByMeshComponent(InMeshComponent);
}

void UArcGISRendererComponent::OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent)
{
	if (MapComponent.IsValid() && MapComponent != InMapComponent)
	{
		MapComponent->OnEditorModeChanged.RemoveAll(this);
	}

	auto MapComponentChanged = false;

	if (MapComponent != InMapComponent)
	{
		MapComponent = InMapComponent;

		MapComponentChanged = true;
	}

	if (MapComponent.IsValid())
	{
		// OnEditorModeChanged gets serialized so we should make sure a unique binding exists
		if (!MapComponent->OnEditorModeChanged.Contains(this, GET_FUNCTION_NAME_CHECKED(UArcGISRendererComponent, OnEditorModeChanged)))
		{
			MapComponent->OnEditorModeChanged.AddUniqueDynamic(this, &UArcGISRendererComponent::OnEditorModeChanged);
		}

		if (!MapComponent->OnMeshCollidersEnabledChanged.Contains(this,
																  GET_FUNCTION_NAME_CHECKED(UArcGISRendererComponent, OnMeshCollidersEnabledChanged)))
		{
			MapComponent->OnMeshCollidersEnabledChanged.AddUniqueDynamic(this, &UArcGISRendererComponent::OnMeshCollidersEnabledChanged);
		}
	}

	if (!MapComponent.IsValid())
	{
		ArcGISRenderer.Reset();

		return;
	}

	if (MapComponentChanged || !ArcGISRenderer)
	{
		if (MapComponent.IsValid() && MapComponent->ShouldEditorComponentBeUpdated())
		{
			ArcGISRenderer = ::MakeShared<Esri::ArcGISMapsSDK::Renderer::FArcGISRenderer>(
				MapComponent->GetView()->APIObject, this, MapComponent->GetAreMeshCollidersEnabled() && GetWorld()->IsGameWorld(),
				bTerrainOcclusionEnabled);
		}
	}
}

void UArcGISRendererComponent::OnEditorModeChanged()
{
	if (MapComponent.IsValid() && !MapComponent->GetIsEditorModeEnabled())
	{
		ArcGISRenderer.Reset();
	}
	else if (MapComponent.IsValid() && MapComponent->ShouldEditorComponentBeUpdated())
	{
		ArcGISRenderer = ::MakeShared<Esri::ArcGISMapsSDK::Renderer::FArcGISRenderer>(
			MapComponent->GetView()->APIObject, this, MapComponent->GetAreMeshCollidersEnabled() && GetWorld()->IsGameWorld(),
			bTerrainOcclusionEnabled);
	}
}

void UArcGISRendererComponent::OnMeshCollidersEnabledChanged()
{
	ArcGISRenderer->SetAreMeshCollidersEnabled(MapComponent->GetAreMeshCollidersEnabled() && GetWorld()->IsGameWorld());
}
