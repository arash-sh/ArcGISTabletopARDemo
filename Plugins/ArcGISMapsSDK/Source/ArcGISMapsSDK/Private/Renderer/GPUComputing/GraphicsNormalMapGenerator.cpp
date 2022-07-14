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
#include "ArcGISMapsSDK/Renderer/GPUComputing/GraphicsNormalMapGenerator.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/GPUComputingUtils.h"

#include "ArcGISMapsSDK/Renderer/GPUComputing/NormalMapGeneratorShaders.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
void FGraphicsNormalMapGenerator::DrawGlobalViewMode(const TSharedPtr<GPUResources::FGPUResourceTexture2D>& InputElevation,
													 const FVector4f& InputOffsetAndScale,
													 float CircleLongitude,
													 float MinLatitude,
													 float LatitudeAngleDelta,
													 float LongitudeArc,
													 float LatitudeLength,
													 const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Output)
{
	ENQUEUE_RENDER_COMMAND(GenerateNormalMapGlobal)
	([InputElevation, InputOffsetAndScale, CircleLongitude, MinLatitude, LatitudeAngleDelta, LongitudeArc, LatitudeLength,
	  Output](FRHICommandListImmediate& RHICmdList) {
		auto OutputNormalMapRHIRef = GPUComputingUtils::GetRHITexture2D(Output);
		RHICmdList.Transition(FRHITransitionInfo(OutputNormalMapRHIRef, ERHIAccess::Unknown, ERHIAccess::RTV));

		FNormalMapGeneratorGlobalViewModePS::FParameters Parameters;

		auto InputElevationRHIRef = GPUComputingUtils::GetRHITexture(InputElevation);
		RHICmdList.Transition(FRHITransitionInfo(InputElevationRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVGraphics));
		Parameters.Input = InputElevationRHIRef;
		Parameters.Sampler = TStaticSamplerState<SF_Bilinear>::GetRHI();

		Parameters.InputOffsetAndScale = InputOffsetAndScale;
		Parameters.CircleLongitude = CircleLongitude;
		Parameters.MinLatitude = MinLatitude;
		Parameters.LatitudeAngleDelta = LatitudeAngleDelta;
		Parameters.LongitudeArc = LongitudeArc;
		Parameters.LatitudeLength = LatitudeLength;
		Parameters.OutputWidth = Output->GetWidth();
		Parameters.OutputHeight = Output->GetHeight();

		FRHIRenderPassInfo RPInfo(OutputNormalMapRHIRef, ERenderTargetActions::Load_Store);

		RHICmdList.BeginRenderPass(RPInfo, TEXT("GenerateNormalMaps"));
		{
			auto VertexShader = GPUComputingUtils::GetShader<FNormalMapGeneratorVS>();
			auto PixelShader = GPUComputingUtils::GetShader<FNormalMapGeneratorGlobalViewModePS>();

			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			{
				RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

				GraphicsPSOInit.BlendState =
					TStaticBlendStateWriteMask<CW_RGBA, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE>::GetRHI();
				GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
				GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GNormalMapGeneratorGlobalViewModePSResources.VertexDeclarationRHI;
				GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
				GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
			}

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), Parameters);

			RHICmdList.SetViewport(0, 0, 0.0f, OutputNormalMapRHIRef->GetSizeX(), OutputNormalMapRHIRef->GetSizeY(), 1.0f);
			RHICmdList.SetStreamSource(0, GNormalMapGeneratorGlobalViewModePSResources.VertexBufferRHI, 0);
			RHICmdList.DrawPrimitive(0, 2, 1);
		}
		RHICmdList.EndRenderPass();

		RHICmdList.Transition(FRHITransitionInfo(OutputNormalMapRHIRef, ERHIAccess::RTV, ERHIAccess::SRVGraphics));
	});
}

void FGraphicsNormalMapGenerator::DrawLocalViewMode(const TSharedPtr<GPUResources::FGPUResourceTexture2D>& InputElevation,
													const FVector4f& InputOffsetAndScale,
													float LatitudeLength,
													float LongitudeLength,
													const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Output)
{
	ENQUEUE_RENDER_COMMAND(GenerateNormalMapLocal)
	([InputElevation, InputOffsetAndScale, LatitudeLength, LongitudeLength, Output](FRHICommandListImmediate& RHICmdList) {
		auto OutputNormalMapRHIRef = GPUComputingUtils::GetRHITexture2D(Output);
		RHICmdList.Transition(FRHITransitionInfo(OutputNormalMapRHIRef, ERHIAccess::Unknown, ERHIAccess::RTV));

		FNormalMapGeneratorLocalViewModePS::FParameters Parameters;

		auto InputElevationRHIRef = GPUComputingUtils::GetRHITexture(InputElevation);
		RHICmdList.Transition(FRHITransitionInfo(InputElevationRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVGraphics));
		Parameters.Input = InputElevationRHIRef;
		Parameters.Sampler = TStaticSamplerState<SF_Bilinear>::GetRHI();

		Parameters.InputOffsetAndScale = InputOffsetAndScale;
		Parameters.LatitudeLength = LatitudeLength;
		Parameters.LongitudeLength = LongitudeLength;
		Parameters.OutputWidth = Output->GetWidth();
		Parameters.OutputHeight = Output->GetHeight();

		FRHIRenderPassInfo RPInfo(OutputNormalMapRHIRef, ERenderTargetActions::Load_Store);

		RHICmdList.BeginRenderPass(RPInfo, TEXT("GenerateNormalMaps"));
		{
			auto VertexShader = GPUComputingUtils::GetShader<FNormalMapGeneratorVS>();
			auto PixelShader = GPUComputingUtils::GetShader<FNormalMapGeneratorLocalViewModePS>();

			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			{
				RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

				GraphicsPSOInit.BlendState =
					TStaticBlendStateWriteMask<CW_RGBA, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE>::GetRHI();
				GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
				GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GNormalMapGeneratorGlobalViewModePSResources.VertexDeclarationRHI;
				GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
				GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
			}

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), Parameters);

			RHICmdList.SetViewport(0, 0, 0.0f, OutputNormalMapRHIRef->GetSizeX(), OutputNormalMapRHIRef->GetSizeY(), 1.0f);
			RHICmdList.SetStreamSource(0, GNormalMapGeneratorGlobalViewModePSResources.VertexBufferRHI, 0);
			RHICmdList.DrawPrimitive(0, 2, 1);
		}
		RHICmdList.EndRenderPass();

		RHICmdList.Transition(FRHITransitionInfo(OutputNormalMapRHIRef, ERHIAccess::RTV, ERHIAccess::SRVGraphics));
	});
}

} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
