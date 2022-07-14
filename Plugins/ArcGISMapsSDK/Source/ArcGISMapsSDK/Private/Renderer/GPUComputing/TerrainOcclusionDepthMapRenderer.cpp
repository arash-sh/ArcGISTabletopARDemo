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
#include "ArcGISMapsSDK/Renderer/GPUComputing/TerrainOcclusionDepthMapRenderer.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Modules/ModuleManager.h"
#include "RHIContext.h"
#include "RenderGraphBuilder.h"
#include "SceneView.h"
#include "Shader.h"

#include "ArcGISMapsSDK/Renderer/GPUComputing/IntegratedMeshClippingShaders.h"
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceProvider.h"
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceRenderTarget.h"
#include "ArcGISMapsSDK/Renderer/SceneComponents/SceneComponentProvider.h"
#include "ArcGISMapsSDK/Utils/ViewportProperties.h"
#include <ArcGISMapsSDK/Public/ArcGISMapsSDK/Utils/Unreal/CameraHelpers.h>

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
FTerrainOcclusionDepthMapRenderer::FTerrainOcclusionDepthMapRenderer(TSharedPtr<SceneComponents::FSceneComponentProvider> InSceneComponentProvider,
																	 TSharedPtr<GPUResources::FGPUResourceProvider> InGpuResourceProvider,
																	 UObject* InOuter)
: SceneComponentProvider(std::move(InSceneComponentProvider)),
  Outer(InOuter)
{
	InitializeCubeAndQuad();

	ViewportSize = {1024, 1024};

	ClippingRenderTarget = NewObject<UTextureRenderTarget2D>();
	ClippingRenderTarget->bAutoGenerateMips = false;
	ClippingRenderTarget->ClearColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	ClippingRenderTarget->InitCustomFormat(1024, 1024, EPixelFormat::PF_R8G8B8A8, true);
	ClippingRenderTarget->UpdateResourceImmediate(true);
	GIntegratedMeshClippingPSResources.InitRHI();
	ClippingRenderTarget->AddToRoot();

	InGpuResourceProvider->SetOcclusionRenderTarget(ClippingRenderTarget);

	MaterialParameterCollection = LoadObject<UMaterialParameterCollection>(
		InOuter, ANSI_TO_TCHAR("/ArcGISMapsSDK/Materials/TerrainOcclusionParameters.TerrainOcclusionParameters"));

	ENQUEUE_RENDER_COMMAND(CreateIMClippingTexture)
	([&](FRHICommandListImmediate& RHICmdList) {
		FRHIResourceCreateInfo CreateInfo(TEXT("IMClippingDepthStencil"), FClearValueBinding(0.f, 0u));
		ClippingDepthStencil = RHICreateTexture2D(1024, 1024, EPixelFormat::PF_DepthStencil, 1, 1,
												  ETextureCreateFlags::DepthStencilTargetable | ETextureCreateFlags::ShaderResource, CreateInfo);
	});
}

FTerrainOcclusionDepthMapRenderer::~FTerrainOcclusionDepthMapRenderer()
{
	BeginReleaseResource(&CubePositionVertexBuffer);
	BeginReleaseResource(&CubeIndexBuffer);

	BeginReleaseResource(&QuadPositionVertexBuffer);
	BeginReleaseResource(&QuadIndexBuffer);

	FRenderCommandFence ReleaseResourcesFence;
	ReleaseResourcesFence.BeginFence();
	ReleaseResourcesFence.Wait();
}

void FTerrainOcclusionDepthMapRenderer::BeginRendering()
{
	if (OnPostOpaqueRenderHandle.IsValid())
	{
		return;
	}

	const FName RendererModuleName("Renderer");
	auto RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);

	if (RendererModule)
	{
		OnPostOpaqueRenderHandle =
			RendererModule->RegisterPostOpaqueRenderDelegate(FPostOpaqueRenderDelegate::CreateRaw(this, &FTerrainOcclusionDepthMapRenderer::Render));
	}
}

void FTerrainOcclusionDepthMapRenderer::EndRendering()
{
	if (!OnPostOpaqueRenderHandle.IsValid())
	{
		return;
	}

	const FName RendererModuleName("Renderer");
	auto RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);

	if (RendererModule)
	{
		RendererModule->RemovePostOpaqueRenderDelegate(OnPostOpaqueRenderHandle);
	}

	OnPostOpaqueRenderHandle.Reset();
}

void FTerrainOcclusionDepthMapRenderer::InitializeCubeAndQuad()
{
	CubePositionVertexBuffer.Init(8);
	CubeIndexBuffer.Indices.SetNum(36);

	QuadPositionVertexBuffer.Init(4);
	QuadIndexBuffer.Indices.SetNum(36);

	const FVector3f CubeVertices[] = {FVector3f(-0.5f, -0.5f, -0.5f), FVector3f(0.5f, -0.5f, -0.5f), FVector3f(-0.5f, 0.5f, -0.5f),
									  FVector3f(0.5f, 0.5f, -0.5f),	  FVector3f(-0.5f, -0.5f, 0.5f), FVector3f(0.5f, -0.5f, 0.5f),
									  FVector3f(-0.5f, 0.5f, 0.5f),	  FVector3f(0.5f, 0.5f, 0.5f)};

	const int32 CubeIndices[] = {0, 1, 2, 3, 1, 2, 4, 1, 0, 4, 1, 5, 5, 1, 3, 5, 3, 7, 6, 4, 5, 6, 5, 7, 2, 0, 4, 6, 2, 4, 2, 7, 3, 2, 6, 7};

	FMemory::Memcpy(CubePositionVertexBuffer.GetVertexData(), CubeVertices, 8 * sizeof(FVector3f));
	FMemory::Memcpy(CubeIndexBuffer.Indices.GetData(), CubeIndices, 36 * sizeof(int32));

	const FVector3f QuadVertices[] = {FVector3f(-1.0f, -1.0f, 0.0f), FVector3f(1.0f, -1.0f, 0.0f), FVector3f(-1.0f, 1.0f, 0.0f),
									  FVector3f(1.0f, 1.0f, 0.0f)};

	const int32 QuadIndices[] = {0, 1, 2, 3, 2, 1};

	FMemory::Memcpy(QuadPositionVertexBuffer.GetVertexData(), QuadVertices, 4 * sizeof(FVector3f));
	FMemory::Memcpy(QuadIndexBuffer.Indices.GetData(), QuadIndices, 6 * sizeof(int32));

	ENQUEUE_RENDER_COMMAND(CreateCubeBuffers)
	([&](FRHICommandListImmediate& RHICmdList) {
		CubePositionVertexBuffer.InitResource();
		CubeIndexBuffer.InitResource();

		QuadPositionVertexBuffer.InitResource();
		QuadIndexBuffer.InitResource();
	});
}

void FTerrainOcclusionDepthMapRenderer::Render(FPostOpaqueRenderParameters& parameters)
{
	// TODO(MF): Use GraphBuilder instead of RHICmdList
	auto& RHICmdList = parameters.GraphBuilder->RHICmdList;

	FTexture2DRHIRef RTRef = ClippingRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();
	FRHITexture2D* RHIRT = RTRef->GetTexture2D();

	// Depth stencil
	FRHITexture2D* RHIDS = ClippingDepthStencil->GetTexture2D();

	RHICmdList.Transition(FRHITransitionInfo(RHIRT, ERHIAccess::Unknown, ERHIAccess::RTV));
	RHICmdList.Transition(FRHITransitionInfo(RHIDS, ERHIAccess::Unknown, ERHIAccess::DSVWrite));

	auto CameraPosition = parameters.ViewMatrix.Inverse().GetOrigin();

	RenderStencilMask(RHICmdList, parameters.ViewMatrix, parameters.ProjMatrix);

	if (IsCameraInsideAnyOBB(CameraPosition))
	{
		ClearDepthToZero(RHICmdList);
	}
	else
	{
		RenderOBBDepth(RHICmdList, parameters.ViewMatrix, parameters.ProjMatrix);
	}

	RHICmdList.Transition(FRHITransitionInfo(RHIRT, ERHIAccess::RTV, ERHIAccess::SRVGraphics));
}

void FTerrainOcclusionDepthMapRenderer::RenderStencilMask(FRHICommandListImmediate& RHICmdList, const FMatrix44d& InView, const FMatrix44d& InProj)
{
	auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	auto VertexShader = TShaderMapRef<FIntegratedMeshClippingVS>(ShaderMap);
	auto PixelShader = TShaderMapRef<FIntegratedMeshClippingStencilPassPS>(ShaderMap);

	const auto& ViewProj = InView.operator*(InProj);

	FTexture2DRHIRef RTRef = ClippingRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();
	FRHITexture2D* RHIRT = RTRef->GetTexture2D();

	FRHITexture2D* RHIDS = ClippingDepthStencil->GetTexture2D();

	FRHIRenderPassInfo RPInfo(RHIRT, ERenderTargetActions::DontLoad_DontStore, RHIDS,
							  EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);

	RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawStencilMask"));
	{
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		{
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			GraphicsPSOInit.BlendState = TStaticBlendStateWriteMask<CW_RGBA, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE>::GetRHI();
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<true, CF_GreaterEqual, true, CF_Always, SO_Keep, SO_Keep, SO_Replace, true,
																		 CF_Always, SO_Keep, SO_Keep, SO_Replace, 0x00, 0xFF>::GetRHI();

			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GIntegratedMeshClippingPSResources.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		}
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 1);

		RHICmdList.SetViewport(0.f, 0.f, 0.f, (float)RHIRT->GetSizeX(), (float)RHIRT->GetSizeY(), 1.0f);

		// Draw all meshes inside the pass after initial setup
		for (const auto& Node : RThreadSceneComponentRenderData)
		{
			FMatrix44d ModelMatrix = FTransform(Node.obb.rot, Node.obb.pos, Node.obb.scale).ToMatrixWithScale();

			FIntegratedMeshClippingVS::FParameters VSParameters;
			VSParameters.MV = Node.m.operator*(FMatrix44f(InView));
			VSParameters.MVP = Node.m.operator*(FMatrix44f(ViewProj));
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), VSParameters);

			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), {});

			RHICmdList.SetStreamSource(0, Node.vb, 0);
			RHICmdList.DrawIndexedPrimitive(Node.ib, 0, 0, Node.vertexCount, 0, Node.faceCount, 1);
		}
	}
	RHICmdList.EndRenderPass();
}

void FTerrainOcclusionDepthMapRenderer::ClearDepthToZero(FRHICommandListImmediate& RHICmdList)
{
	auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	auto VertexShader = TShaderMapRef<FIntegratedMeshClippingVS>(ShaderMap);
	auto PixelShader = TShaderMapRef<FIntegratedMeshClippingClearToZeroPassPS>(ShaderMap);

	FTexture2DRHIRef RTRef = ClippingRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();
	FRHITexture2D* RHIRT = RTRef->GetTexture2D();

	FRHITexture2D* RHIDS = ClippingDepthStencil->GetTexture2D();

	FRHIRenderPassInfo RPInfo(RHIRT, ERenderTargetActions::Clear_Store, RHIDS, EDepthStencilTargetActions::LoadDepthStencil_StoreDepthStencil);

	RHICmdList.BeginRenderPass(RPInfo, TEXT("ClearToZero"));
	{
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		{
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			GraphicsPSOInit.BlendState = TStaticBlendStateWriteMask<CW_RGBA, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE>::GetRHI();
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always, true, CF_LessEqual, SO_Keep, SO_Keep, SO_Replace, true,
																		 CF_LessEqual, SO_Keep, SO_Keep, SO_Replace, 0xFF, 0x00>::GetRHI();

			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GIntegratedMeshClippingPSResources.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		}
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 1);

		RHICmdList.SetViewport(0.f, 0.f, 0.f, (float)RHIRT->GetSizeX(), (float)RHIRT->GetSizeY(), 1.0f);

		FIntegratedMeshClippingVS::FParameters VSParameters;
		VSParameters.MV = FMatrix44f::Identity;
		VSParameters.MVP = FMatrix44f::Identity;
		SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), VSParameters);

		SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), {});

		RHICmdList.SetStreamSource(0, QuadPositionVertexBuffer.VertexBufferRHI, 0);
		RHICmdList.DrawIndexedPrimitive(QuadIndexBuffer.IndexBufferRHI, 0, 0, 4, 0, 2, 1);
	}
	RHICmdList.EndRenderPass();
}

void FTerrainOcclusionDepthMapRenderer::RenderOBBDepth(FRHICommandListImmediate& RHICmdList, const FMatrix44d& InView, const FMatrix44d& InProj)
{
	auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	auto VertexShader = TShaderMapRef<FIntegratedMeshClippingVS>(ShaderMap);
	auto PixelShader = TShaderMapRef<FIntegratedMeshClippingOBBDepthPassPS>(ShaderMap);

	const auto& ViewProj = InView.operator*(InProj);

	FTexture2DRHIRef RTRef = ClippingRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();
	FRHITexture2D* RHIRT = RTRef->GetTexture2D();

	FRHITexture2D* RHIDS = ClippingDepthStencil->GetTexture2D();

	FRHIRenderPassInfo RPInfo(RHIRT, ERenderTargetActions::Clear_Store, RHIDS, EDepthStencilTargetActions::LoadDepthStencil_StoreDepthStencil);

	RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawOBBDepth"));
	{
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		{
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

			GraphicsPSOInit.BlendState = TStaticBlendStateWriteMask<CW_RGBA, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE>::GetRHI();
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<true, CF_GreaterEqual, true, CF_LessEqual, SO_Keep, SO_Keep, SO_Replace,
																		 true, CF_LessEqual, SO_Keep, SO_Keep, SO_Replace, 0xFF, 0x00>::GetRHI();

			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GIntegratedMeshClippingPSResources.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		}
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 1);

		RHICmdList.SetViewport(0.f, 0.f, 0.f, (float)RHIRT->GetSizeX(), (float)RHIRT->GetSizeY(), 1.0f);

		// TODO(MF): Use instancing
		for (const auto& Node : RThreadSceneComponentRenderData)
		{
			FMatrix44d ModelMatrix = FTransform(Node.obb.rot, Node.obb.pos, Node.obb.scale).ToMatrixWithScale();

			FIntegratedMeshClippingVS::FParameters VSParameters;
			VSParameters.MV = FMatrix44f(ModelMatrix.operator*(InView));
			VSParameters.MVP = FMatrix44f(ModelMatrix.operator*(ViewProj));
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), VSParameters);

			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), {});

			RHICmdList.SetStreamSource(0, CubePositionVertexBuffer.VertexBufferRHI, 0);
			RHICmdList.DrawIndexedPrimitive(CubeIndexBuffer.IndexBufferRHI, 0, 0, 8, 0, 12, 1);
		}
	}
	RHICmdList.EndRenderPass();
}

bool FTerrainOcclusionDepthMapRenderer::IsCameraInsideAnyOBB(const FVector3d& InCameraPosition)
{
	for (const auto& Node : RThreadSceneComponentRenderData)
	{
		auto InverseOBBTransform = FTransform(Node.obb.rot, Node.obb.pos, Node.obb.scale).Inverse();
		auto CameraPositionInLocalOBB = InverseOBBTransform.TransformPosition(InCameraPosition);

		if (FMath::Abs(CameraPositionInLocalOBB.X) < 0.5 && FMath::Abs(CameraPositionInLocalOBB.Y) < 0.5 &&
			FMath::Abs(CameraPositionInLocalOBB.Z) < 0.5)
		{
			return true;
		}
	}

	return false;
}

void FTerrainOcclusionDepthMapRenderer::Update(const FViewportProperties& InViewportProperties)
{
	// Ensure the render target is the same size as the viewport
	auto NewViewportWidth = InViewportProperties.viewportWidthPixels;
	auto NewViewportHeight = InViewportProperties.viewportHeightPixels;

	if ((ViewportSize.X != NewViewportWidth || ViewportSize.Y != NewViewportHeight) && NewViewportWidth && NewViewportHeight)
	{
		ViewportSize = {NewViewportWidth, NewViewportHeight};

		ClippingRenderTarget->ResizeTarget(NewViewportWidth, NewViewportHeight);
		ClippingRenderTarget->UpdateResourceImmediate();

		ENQUEUE_RENDER_COMMAND(ResizeClippingDepthStencil)
		([&, NewViewportWidth, NewViewportHeight](FRHICommandListImmediate& RHICmdList) {
			FRHIResourceCreateInfo CreateInfo(TEXT("IMClippingDepthStencil"), FClearValueBinding(0.f, 0u));
			ClippingDepthStencil = RHICreateTexture2D(NewViewportWidth, NewViewportHeight, EPixelFormat::PF_DepthStencil, 1, 1,
													  ETextureCreateFlags::DepthStencilTargetable | ETextureCreateFlags::ShaderResource, CreateInfo);
		});
	}

	if (Outer && Outer->GetWorld())
	{
		if (auto MaterialParameterCollectionInstance = Outer->GetWorld()->GetParameterCollectionInstance(MaterialParameterCollection))
		{
			auto ViewProj = UCameraHelpers::GetActiveCameraViewProjMatrix(Outer);

			MaterialParameterCollectionInstance->SetVectorParameterValue(
				"TerrainOcclusionViewProjRow0", FVector4d(ViewProj.M[0][0], ViewProj.M[0][1], ViewProj.M[0][2], ViewProj.M[0][3]));
			MaterialParameterCollectionInstance->SetVectorParameterValue(
				"TerrainOcclusionViewProjRow1", FVector4d(ViewProj.M[1][0], ViewProj.M[1][1], ViewProj.M[1][2], ViewProj.M[1][3]));
			MaterialParameterCollectionInstance->SetVectorParameterValue(
				"TerrainOcclusionViewProjRow2", FVector4d(ViewProj.M[2][0], ViewProj.M[2][1], ViewProj.M[2][2], ViewProj.M[2][3]));
			MaterialParameterCollectionInstance->SetVectorParameterValue(
				"TerrainOcclusionViewProjRow3", FVector4d(ViewProj.M[3][0], ViewProj.M[3][1], ViewProj.M[3][2], ViewProj.M[3][3]));
		}
	}

	TArray<FCachedSCRenderData> SceneComponentRenderData;

	const TMap<uint32, TSharedPtr<SceneComponents::FSceneComponent>>& SceneComponents = SceneComponentProvider->GetSceneComponents();

	for (const auto& SceneComponentIter : SceneComponents)
	{
		const auto& SceneComponent = SceneComponentIter.Value;
		if (SceneComponent->GetVisible() && SceneComponent->GetMasksTerrain())
		{
			auto VertexAndIndedBuffer = SceneComponent->GetPositionBuffer();

			auto VertexBuffer = VertexAndIndedBuffer.first;
			auto IndexBuffer = VertexAndIndedBuffer.second;
			if (VertexBuffer->IsInitialized() && IndexBuffer->IsInitialized())
			{
				auto ModelMat = FMatrix44f(SceneComponent->GetTransform().ToMatrixWithScale());
				SceneComponentRenderData.Add(FCachedSCRenderData{VertexBuffer->VertexBufferRHI, VertexBuffer->GetNumVertices(),
																 IndexBuffer->IndexBufferRHI, static_cast<uint32_t>(IndexBuffer->Indices.Num()) / 3u,
																 ModelMat, SceneComponent->GetObbTransform()});
			}
		}
	}

	// Next lambda can be executed before or after Render method
	ENQUEUE_RENDER_COMMAND(UpdateSceneComponentRenderData)
	([&, SceneComponentRenderData = std::move(SceneComponentRenderData)](FRHICommandListImmediate& RHICmdList) {
		RThreadSceneComponentRenderData = std::move(SceneComponentRenderData);
	});
}

UTextureRenderTarget2D* FTerrainOcclusionDepthMapRenderer::GetClippingRenderTarget()
{
	return ClippingRenderTarget;
}
} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
