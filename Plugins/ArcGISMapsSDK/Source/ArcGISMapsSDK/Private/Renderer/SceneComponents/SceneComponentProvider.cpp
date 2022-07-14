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
#include "ArcGISMapsSDK/Renderer/SceneComponents/SceneComponentProvider.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace SceneComponents
{
FSceneComponentProvider::FSceneComponentProvider(UObject* InOuter, bool bAreMeshCollidersEnabled) : Outer(InOuter), bAreMeshCollidersEnabled(bAreMeshCollidersEnabled)
{
}

TSharedPtr<FSceneComponent> FSceneComponentProvider::CreateSceneComponent(uint32 InId, uint32 InLayerId)
{
	auto NewSceneComponent = ::MakeShared<FSceneComponent>(Outer, InLayerId);
	NewSceneComponent->SetIsMeshColliderEnabled(bAreMeshCollidersEnabled);

	SceneComponents.Emplace(InId, NewSceneComponent);
	SceneComponentsByMeshComponent.Emplace(Cast<UMeshComponent>(NewSceneComponent->GetMeshComponent()), NewSceneComponent);

	return NewSceneComponent;
}

TSharedPtr<FSceneComponent> FSceneComponentProvider::GetSceneComponent(uint32 InId)
{
	const auto SceneComponentIt = SceneComponents.Find(InId);
	return SceneComponentIt ? *SceneComponentIt : nullptr;
}

const TMap<uint32, TSharedPtr<FSceneComponent>>& FSceneComponentProvider::GetSceneComponents()
{
	return SceneComponents;
}

const TSharedPtr<const FSceneComponent> FSceneComponentProvider::GetSceneComponentFrom(const UMeshComponent* InMeshComponent) const
{
	const auto SceneComponentIt = SceneComponentsByMeshComponent.Find(InMeshComponent);
	return SceneComponentIt ? *SceneComponentIt : nullptr;
}

void FSceneComponentProvider::DestroySceneComponent(uint32 InId)
{
	const auto SceneComponentIt = SceneComponents.Find(InId);

	if (SceneComponentIt)
	{
		SceneComponentsByMeshComponent.Remove(Cast<UMeshComponent>((*SceneComponentIt)->GetMeshComponent()));
		SceneComponents.Remove(InId);
	}
}

bool FSceneComponentProvider::GetAreMeshCollidersEnabled() const
{
	return false;
}

void FSceneComponentProvider::SetAreMeshCollidersEnabled(bool bInEnabled)
{
	if (bAreMeshCollidersEnabled != bInEnabled)
	{
		bAreMeshCollidersEnabled = bInEnabled;

		for (auto& SceneComponent : SceneComponents)
		{
			SceneComponent.Value->SetIsMeshColliderEnabled(bInEnabled);
		}
	}
}
} // namespace SceneComponents
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
