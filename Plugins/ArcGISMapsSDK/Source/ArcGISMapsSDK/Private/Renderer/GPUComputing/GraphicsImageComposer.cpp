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
#include "ArcGISMapsSDK/Renderer/GPUComputing/GraphicsImageComposer.h"

#include "ArcGISMapsSDK/Renderer/GPUComputing/ImageComposerShaders.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
void FGraphicsImageComposer::Draw(std::vector<TSharedPtr<const GPUResources::FGPUResourceTexture>> Inputs,
								  std::vector<FVector4f> OffsetScaleOpacity,
								  const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Output)
{
	check(Inputs.size() == OffsetScaleOpacity.size());

	ENQUEUE_RENDER_COMMAND(ComposeImagery)
	([this, Inputs, OffsetScaleOpacity, Output](FRHICommandListImmediate& RHICmdList) {
		auto* OutputTexture = Cast<UTextureRenderTarget2D>(Output->GetNativeTexture());
		FTexture2DRHIRef OutputRHIRef = (OutputTexture && OutputTexture->GetResource() && OutputTexture->GetResource()->TextureRHI) ?
											OutputTexture->GetResource()->TextureRHI->GetTexture2D() :
											nullptr;

		if (!OutputRHIRef)
		{
			UE_LOG(LogTemp, Error, TEXT("Attempting to compose to an output texture %s with no texture 2D RHI"), *OutputTexture->GetName());
			return;
		}

		RHICmdList.Transition(FRHITransitionInfo(OutputRHIRef, ERHIAccess::Unknown, ERHIAccess::RTV));

		FImageComposerPS::FParameters Parameters;
		{
			Parameters.NumTextures = Inputs.size();

			for (uint32 i = 0; i < Inputs.size(); ++i)
			{
				auto* Texture = Inputs[i]->GetNativeTexture();
				auto TextureRHIRef = Texture && Texture->GetResource() ? Texture->GetResource()->TextureRHI : nullptr;

				Parameters.OffsetScaleOpacity[i] = OffsetScaleOpacity[i];
				Parameters.Samplers[i] = TStaticSamplerState<SF_Bilinear>::GetRHI();
				Parameters.Textures[i] = TextureRHIRef;

				RHICmdList.Transition(FRHITransitionInfo(TextureRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVGraphics));
			}

			for (uint32 i = Inputs.size(); i < 8; ++i)
			{
				Parameters.Samplers[i] = TStaticSamplerState<SF_Point>::GetRHI();
				Parameters.Textures[i] = GBlackTexture->TextureRHI;
			}
		}

		FRHIRenderPassInfo RPInfo(OutputRHIRef, ERenderTargetActions::Load_Store);

		RHICmdList.BeginRenderPass(RPInfo, TEXT("ComposeImagery"));
		{
			auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

			TShaderMapRef<FImageComposerVS> VertexShader(ShaderMap);
			TShaderMapRef<FImageComposerPS> PixelShader(ShaderMap);

			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			{
				RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

				GraphicsPSOInit.BlendState =
					TStaticBlendStateWriteMask<CW_RGBA, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE>::GetRHI();
				GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
				GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
				GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GImageComposerPSResources.VertexDeclarationRHI;
				GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
				GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
				GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
			}

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), Parameters);

			RHICmdList.SetViewport(0, 0, 0.0f, OutputRHIRef->GetSizeX(), OutputRHIRef->GetSizeY(), 1.0f);
			RHICmdList.SetStreamSource(0, GImageComposerPSResources.VertexBufferRHI, 0);
			RHICmdList.DrawPrimitive(0, 2, 1);
		}
		RHICmdList.EndRenderPass();

		RHICmdList.Transition(FRHITransitionInfo(OutputRHIRef, ERHIAccess::RTV, ERHIAccess::SRVGraphics));
	});
}
} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
