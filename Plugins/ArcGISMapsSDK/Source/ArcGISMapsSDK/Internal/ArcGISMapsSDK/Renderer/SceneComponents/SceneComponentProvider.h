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
#pragma once

#include "ArcGISMapsSDK/Renderer/SceneComponents/SceneComponent.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace SceneComponents
{
class FSceneComponentProvider
{
public:
	FSceneComponentProvider(UObject* InOuter, bool bAreMeshCollidersEnabled);

	TSharedPtr<FSceneComponent> CreateSceneComponent(uint32 InId, uint32 InLayerId);

	TSharedPtr<FSceneComponent> GetSceneComponent(uint32 InId);
	const TMap<uint32, TSharedPtr<FSceneComponent>>& GetSceneComponents();

	const TSharedPtr<const FSceneComponent> GetSceneComponentFrom(const UMeshComponent* InMeshComponent) const;

	void DestroySceneComponent(uint32 InId);

	bool GetAreMeshCollidersEnabled() const;
	void SetAreMeshCollidersEnabled(bool bInEnabled);

private:
	UObject* Outer;

	TMap<uint32, TSharedPtr<FSceneComponent>> SceneComponents;
	TMap<const UMeshComponent*, TSharedPtr<FSceneComponent>> SceneComponentsByMeshComponent;

	bool bAreMeshCollidersEnabled = false;
};
} // namespace SceneComponents
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
