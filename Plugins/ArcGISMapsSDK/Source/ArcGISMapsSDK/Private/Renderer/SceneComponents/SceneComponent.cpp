// COPYRIGHT 1995-2021 ESRI
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
#include "ArcGISMapsSDK/Renderer/SceneComponents/SceneComponent.h"

#include "Async/Async.h"

#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceMaterial.h"
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceMesh.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace SceneComponents
{
FSceneComponent::FSceneComponent(UObject* InOuter, uint32 InLayerId) : LayerId(InLayerId), Outer(InOuter)
{
	CreateMeshComponent();
}

FSceneComponent::~FSceneComponent()
{
	if (ArcGISMeshComponent && ArcGISMeshComponent->IsValidLowLevel())
	{
		ArcGISMeshComponent->ConditionalBeginDestroy();
	}
}

void FSceneComponent::CreateMeshComponent()
{
	if (!ArcGISMeshComponent)
	{
		ArcGISMeshComponent = NewObject<UArcGISMeshComponent>(Outer);

		ArcGISMeshComponent->SetFlags(RF_Transient | RF_DuplicateTransient | RF_TextExportTransient);

		ArcGISMeshComponent->CastShadow = bShadows ? 1 : 0;

		ArcGISMeshComponent->RegisterComponent();
	}

	ArcGISMeshComponent->SetInternalVisibility(false);

	bVisible = false;
}

bool FSceneComponent::GetIsMeshColliderEnabled() const
{
	return ArcGISMeshComponent->GetIsMeshColliderEnabled();
}

void FSceneComponent::SetIsMeshColliderEnabled(bool bEnabled)
{
	ArcGISMeshComponent->SetIsMeshColliderEnabled(bEnabled);
}

bool FSceneComponent::GetMasksTerrain() const
{
	return bClipsTerrain;
}

void FSceneComponent::SetMasksTerrain(bool inClipsTerrain)
{
	bClipsTerrain = inClipsTerrain;
}

const ObbTransform& FSceneComponent::GetObbTransform()
{
	return ObbTransform;
}

void FSceneComponent::SetObbTransform(const ::ObbTransform& InTransform)
{
	ObbTransform = InTransform;
}

uint32 FSceneComponent::GetLayerId() const
{
	return LayerId;
}

const UArcGISMeshComponent* FSceneComponent::GetMeshComponent() const
{
	return ArcGISMeshComponent;
}

void FSceneComponent::SetShadows(bool bUseShadows)
{
	if (bShadows != bUseShadows)
	{
		bShadows = bUseShadows;
		ArcGISMeshComponent->CastShadow = bShadows ? 1 : 0;
	}
}

void FSceneComponent::SetVisible(bool bIsVisible)
{
	if (bVisible != bIsVisible)
	{
		bVisible = bIsVisible;

		ArcGISMeshComponent->SetInternalVisibility(bIsVisible);
	}
}

void FSceneComponent::SetMaterial(const TSharedPtr<const GPUResources::FGPUResourceMaterial>& GPUResourceMaterial)
{
	if (ArcGISMeshComponent)
	{
		ArcGISMeshComponent->SetMaterial(0, GPUResourceMaterial->GetInstance());
	}
}

void FSceneComponent::SetMesh(const TSharedPtr<const GPUResources::FGPUResourceMesh>& InGPUResourceMesh)
{
	// Set the mesh asynchronously as SetMesh can take time to execute and may affect throttling
	TWeakPtr<FSceneComponent> Self = AsShared();
	AsyncTask(ENamedThreads::GameThread, [Self, GPUResourceMesh = std::move(InGPUResourceMesh)]() {
		// It may happen the SceneComponent gets destroyed in-between
		if (auto SceneComponent = Self.Pin())
		{
			SceneComponent->ArcGISMeshComponent->SetMesh(GPUResourceMesh);
		}
	});
}

FVector3d FSceneComponent::GetLocation()
{
	return ArcGISMeshComponent->GetComponentLocation();
}

void FSceneComponent::SetLocation(const FVector3d& NewLocation)
{
	ArcGISMeshComponent->SetWorldLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

void FSceneComponent::SetScale(const FVector3d& NewScale)
{
	ArcGISMeshComponent->SetWorldScale3D(NewScale);
}

void FSceneComponent::SetRotation(const FQuat& NewRotation)
{
	ArcGISMeshComponent->SetWorldRotation(NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
}
} // namespace SceneComponents
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
