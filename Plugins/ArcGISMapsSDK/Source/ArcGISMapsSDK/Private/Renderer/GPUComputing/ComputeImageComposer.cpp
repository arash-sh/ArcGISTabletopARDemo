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
#include "ArcGISMapsSDK/Renderer/GPUComputing/ComputeImageComposer.h"

#include "RenderGraphUtils.h"

#include "ArcGISMapsSDK/Renderer/GPUComputing/ImageComposerShaders.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
void FComputeImageComposer::Draw(std::vector<TSharedPtr<const GPUResources::FGPUResourceTexture>> Inputs,
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

		RHICmdList.Transition(FRHITransitionInfo(OutputRHIRef, ERHIAccess::Unknown, ERHIAccess::UAVMask));

		FImageComposerCS::FParameters Parameters;
		{
			Parameters.NumTextures = Inputs.size();

			for (uint32 i = 0; i < Inputs.size(); ++i)
			{
				auto* Texture = Inputs[i]->GetNativeTexture();
				auto TextureRHIRef = Texture && Texture->GetResource() ? Texture->GetResource()->TextureRHI : nullptr;

				Parameters.OffsetScaleOpacity[i] = OffsetScaleOpacity[i];
				Parameters.Samplers[i] = TStaticSamplerState<SF_Bilinear>::GetRHI();
				Parameters.Textures[i] = TextureRHIRef;

				RHICmdList.Transition(FRHITransitionInfo(TextureRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVMask));
			}

			for (uint32 i = Inputs.size(); i < 8; ++i)
			{
				Parameters.Samplers[i] = TStaticSamplerState<SF_Point>::GetRHI();
				Parameters.Textures[i] = GBlackTexture->TextureRHI;
			}

			Parameters.Output = static_cast<FTextureRenderTarget2DResource*>(OutputTexture->GetRenderTargetResource())->GetUnorderedAccessViewRHI();
		}

		{
			auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

			TShaderMapRef<FImageComposerCS> ComputeShader(ShaderMap);

			const uint32 ThreadGroupCountValueX = OutputRHIRef->GetSizeX() / FImageComposerCS::THREADGROUP_SIZE_X;
			const uint32 ThreadGroupCountValueY = FMath::DivideAndRoundUp(OutputRHIRef->GetSizeY(), FImageComposerCS::THREADGROUP_SIZE_Y);

			FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, Parameters, FIntVector(ThreadGroupCountValueX, ThreadGroupCountValueY, 1));
		}

		RHICmdList.Transition(FRHITransitionInfo(OutputRHIRef, ERHIAccess::UAVMask, ERHIAccess::SRVMask));
	});
}
} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
