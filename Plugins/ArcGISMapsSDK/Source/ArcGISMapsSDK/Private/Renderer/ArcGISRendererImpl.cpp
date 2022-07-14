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
#include "Materials/MaterialParameterCollection.h"

#include "ArcGISMapsSDK/Renderer/ArcGISRendererImpl.h"

#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISDecodedRenderCommandQueue.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISRenderCommandServer.h"
#include "ArcGISMapsSDK/API/GameEngine/View/ArcGISView.h"
#include "ArcGISMapsSDK/API/GameEngine/View/ArcGISViewInternal.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Renderer/ArcGISRenderCommandThrottle.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/ComputeImageComposer.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/ComputeNormalMapGenerator.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/GraphicsImageComposer.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/GraphicsNormalMapGenerator.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/TerrainOcclusionDepthMapRenderer.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
FArcGISRenderer::Impl::Impl(TSharedPtr<Esri::GameEngine::View::ArcGISView> InView,
							UObject* InOuter,
							bool bInAreMeshCollidersEnabled,
							bool bInIsTerrainOcclusionEnabled)
{
	EShaderPlatform ShaderPlatform = GetFeatureLevelShaderPlatform(GMaxRHIFeatureLevel);

	if (RHISupportsComputeShaders(ShaderPlatform))
	{
		UE_LOG(LogTemp, Display, TEXT("Using FComputeImageComposer and FComputeNormalMapGenerator"));

		ImageComposer = ::MakeShared<GPUComputing::FComputeImageComposer>();
		NormalMapGenerator = ::MakeShared<GPUComputing::FComputeNormalMapGenerator>(InView);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Using FGraphicsImageComposer and FGraphicsNormalMapGenerator"));

		ImageComposer = ::MakeShared<GPUComputing::FGraphicsImageComposer>();
		NormalMapGenerator = ::MakeShared<GPUComputing::FGraphicsNormalMapGenerator>(InView);
	}

	GPUResourceProvider = ::MakeShared<GPUResources::FGPUResourceProvider>(InOuter);
	SceneComponentProvider = ::MakeShared<SceneComponents::FSceneComponentProvider>(InOuter, bInAreMeshCollidersEnabled);

	if (bInIsTerrainOcclusionEnabled)
	{
		TerrainOcclusionDepthMapRenderer =
			::MakeShared<GPUComputing::FTerrainOcclusionDepthMapRenderer>(SceneComponentProvider, GPUResourceProvider, InOuter);
		TerrainOcclusionDepthMapRenderer->BeginRendering();
	}

	RenderCommandClient =
		::MakeShared<FArcGISRenderCommandClient>(InOuter, GPUResourceProvider, SceneComponentProvider, ImageComposer, NormalMapGenerator);

	View = std::move(InView);
	MapComponent = UArcGISMapComponent::GetMapComponent(InOuter);

	LastENUToWorldTransformAtOrigin = MapComponent->GetENUToWorldTransformAtOrigin();
}

FArcGISRenderer::Impl::~Impl()
{
	if (TerrainOcclusionDepthMapRenderer)
	{
		TerrainOcclusionDepthMapRenderer->EndRendering();
	}
}

void FArcGISRenderer::Impl::Update()
{
	if (TerrainOcclusionDepthMapRenderer)
	{
		TerrainOcclusionDepthMapRenderer->Update(MapComponent->GetViewportProperties());
	}

	if /*constexpr*/ (!FArcGISRenderCommandThrottle::bEnableThrottle)
	{
		// Process one full command queue at a time
		CurrentCommandQueue = Esri::GameEngine::RenderCommandQueue::ArcGISDecodedRenderCommandQueue(
			Esri::GameEngine::View::ArcGISViewInternal::GetRenderCommandServer(View.Get()).GetRenderCommands());
		auto RenderCommand = CurrentCommandQueue.GetNextCommand();
		while (RenderCommand != nullptr)
		{
			RenderCommandClient->ExecuteRenderCommand(*RenderCommand);
			RenderCommand = CurrentCommandQueue.GetNextCommand();
		}
	}
	else
	{
		FArcGISRenderCommandThrottle RenderCommandThrottle;

		// Pick up next command from previous queue, if any
		auto RenderCommand = CurrentCommandQueue.GetNextCommand();
		if (RenderCommand == nullptr)
		{
			CurrentCommandQueue = Esri::GameEngine::RenderCommandQueue::ArcGISDecodedRenderCommandQueue(
				Esri::GameEngine::View::ArcGISViewInternal::GetRenderCommandServer(View.Get()).GetRenderCommands());
		}
		while (RenderCommand != nullptr)
		{
			RenderCommandClient->ExecuteRenderCommand(*RenderCommand);
			if (RenderCommandThrottle.DoThrottle(*RenderCommand))
			{
				// Break and defer processing of the remaining commands to next Update
				break;
			}
			RenderCommand = CurrentCommandQueue.GetNextCommand();
		}
	}

	auto CurrentENUToWorldTransformAtOrigin = MapComponent->GetENUToWorldTransformAtOrigin();

	if (!CurrentENUToWorldTransformAtOrigin.Equals(LastENUToWorldTransformAtOrigin, 0.0f))
	{
		UpdateTransform(SceneComponentProvider->GetSceneComponents(), LastENUToWorldTransformAtOrigin, CurrentENUToWorldTransformAtOrigin);

		LastENUToWorldTransformAtOrigin = CurrentENUToWorldTransformAtOrigin;
	}
}

void FArcGISRenderer::Impl::UpdateTransform(const TMap<uint32, TSharedPtr<SceneComponents::FSceneComponent>>& SceneComponents,
											const FTransform& LastTransform,
											const FTransform& CurrentTransform)
{
	for (const auto& SceneComponentPair : SceneComponents)
	{
		auto SceneComponent = SceneComponentPair.Value;

		SceneComponent->SetRotation(CurrentTransform.GetRotation());

		auto LastSceneComponentPosition = SceneComponent->GetLocation();

		auto LastTransformToCurrentTransform = LastTransform.Inverse() * CurrentTransform;
		auto CurrentSceneComponentLocation = LastTransformToCurrentTransform.TransformPosition(LastSceneComponentPosition);

		SceneComponent->SetLocation(CurrentSceneComponentLocation);
	}
}

uint32 FArcGISRenderer::Impl::GetLayerIdByMeshComponent(const UMeshComponent* InMeshComponent) const
{
	const auto& SceneComponent = SceneComponentProvider->GetSceneComponentFrom(InMeshComponent);

	return SceneComponent ? SceneComponent->GetLayerId() : 0;
}

bool FArcGISRenderer::Impl::GetAreMeshCollidersEnabled() const
{
	return SceneComponentProvider->GetAreMeshCollidersEnabled();
}

void FArcGISRenderer::Impl::SetAreMeshCollidersEnabled(bool bEnabled)
{
	SceneComponentProvider->SetAreMeshCollidersEnabled(bEnabled);
}
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
