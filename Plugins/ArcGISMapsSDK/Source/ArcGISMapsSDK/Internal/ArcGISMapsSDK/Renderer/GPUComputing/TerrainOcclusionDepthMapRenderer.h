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

#include "Materials/MaterialParameterCollection.h"
#include "Templates/SharedPointer.h"
#include "Templates/UniquePtr.h"

#include "ArcGISMapsSDK/Renderer/SceneComponents/SceneComponent.h"
#include "ArcGISMapsSDK/Utils/ViewportProperties.h"

struct ViewportProperties;

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUResources
{
class FGPUResourceProvider;
class FGPUResourceRenderTarget;
} // namespace GPUResources

namespace SceneComponents
{
class FSceneComponentProvider;
} // namespace SceneComponents

namespace GPUComputing
{
class FTerrainOcclusionDepthMapRenderer
{
public:
	FTerrainOcclusionDepthMapRenderer(TSharedPtr<SceneComponents::FSceneComponentProvider> InSceneComponentProvider,
									  TSharedPtr<GPUResources::FGPUResourceProvider> InGpuResourceProvider,
									  UObject* InOuter);

	~FTerrainOcclusionDepthMapRenderer();

	void Update(const FViewportProperties& InViewportProperties);

	UTextureRenderTarget2D* GetClippingRenderTarget();

	void BeginRendering();
	void EndRendering();

private:
	void Render(FPostOpaqueRenderParameters& parameters);
	void ClearDepthToZero(FRHICommandListImmediate& RHICmdList);
	void RenderStencilMask(FRHICommandListImmediate& RHICmdList, const FMatrix44d& InView, const FMatrix44d& InProj);
	void RenderOBBDepth(FRHICommandListImmediate& RHICmdList, const FMatrix44d& InView, const FMatrix44d& InProj);

	void InitializeCubeAndQuad();
	bool IsCameraInsideAnyOBB(const FVector3d& InCameraPosition);

	FDelegateHandle OnPostOpaqueRenderHandle;

	UMaterialParameterCollection* MaterialParameterCollection;

	FDynamicMeshIndexBuffer32 CubeIndexBuffer;
	FPositionVertexBuffer CubePositionVertexBuffer;
	FDynamicMeshIndexBuffer32 QuadIndexBuffer;
	FPositionVertexBuffer QuadPositionVertexBuffer;

	UTextureRenderTarget2D* ClippingRenderTarget;
	FTexture2DRHIRef ClippingDepthStencil;
	TSharedPtr<SceneComponents::FSceneComponentProvider> SceneComponentProvider;

	UObject* Outer;

	struct FCachedSCRenderData
	{
		FBufferRHIRef vb;
		unsigned int vertexCount;
		FBufferRHIRef ib;
		unsigned int faceCount;
		FMatrix44f m;
		ObbTransform obb;
	};

	TArray<FCachedSCRenderData> RThreadSceneComponentRenderData;

	FIntPoint ViewportSize;
};
} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
