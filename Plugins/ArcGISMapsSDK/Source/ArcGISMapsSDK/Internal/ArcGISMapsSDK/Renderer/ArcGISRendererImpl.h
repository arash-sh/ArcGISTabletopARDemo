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
#pragma once

#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISDecodedRenderCommandQueue.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Renderer/ArcGISRenderCommandClient.h"
#include "ArcGISMapsSDK/Renderer/ArcGISRenderer.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
class FTerrainOcclusionDepthMapRenderer;
}

class FArcGISRenderer::Impl
{
public:
	Impl(TSharedPtr<GameEngine::View::ArcGISView> InView, UObject* InOuter, bool bInAreMeshCollidersEnabled, bool bInIsTerrainOcclusionEnabled);
	~Impl();

	void Update();

	uint32 GetLayerIdByMeshComponent(const UMeshComponent* InMeshComponent) const;

	bool GetAreMeshCollidersEnabled() const;
	void SetAreMeshCollidersEnabled(bool bEnabled);

private:
	static void UpdateTransform(const TMap<uint32, TSharedPtr<SceneComponents::FSceneComponent>>& SceneComponents,
								const FTransform& LastTransform,
								const FTransform& CurrentTransform);

	Esri::GameEngine::RenderCommandQueue::ArcGISDecodedRenderCommandQueue CurrentCommandQueue;
	TSharedPtr<FArcGISRenderCommandClient> RenderCommandClient;

	TSharedPtr<GameEngine::View::ArcGISView> View;

	UArcGISMapComponent* MapComponent{nullptr};
	FTransform LastENUToWorldTransformAtOrigin = FTransform::Identity;

	TSharedPtr<GPUResources::FGPUResourceProvider> GPUResourceProvider;
	TSharedPtr<SceneComponents::FSceneComponentProvider> SceneComponentProvider;

	TSharedPtr<GPUComputing::IImageComposer> ImageComposer;
	TSharedPtr<GPUComputing::INormalMapGenerator> NormalMapGenerator;

	TSharedPtr<GPUComputing::FTerrainOcclusionDepthMapRenderer> TerrainOcclusionDepthMapRenderer;
};
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
