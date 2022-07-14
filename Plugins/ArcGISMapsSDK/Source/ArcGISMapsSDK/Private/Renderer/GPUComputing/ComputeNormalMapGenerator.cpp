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
#include "ArcGISMapsSDK/Renderer/GPUComputing/ComputeNormalMapGenerator.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/GPUComputingUtils.h"

#include "RenderGraphUtils.h"

#include "ArcGISMapsSDK/Renderer/GPUComputing/NormalMapGeneratorShaders.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
void FComputeNormalMapGenerator::DrawGlobalViewMode(const TSharedPtr<GPUResources::FGPUResourceTexture2D>& InputElevation,
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
		FNormalMapGeneratorGlobalViewModeCS::FParameters parameters;

		auto TextureRHIRef = GPUComputingUtils::GetRHITexture(InputElevation);
		RHICmdList.Transition(FRHITransitionInfo(TextureRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVCompute));
		parameters.Input = TextureRHIRef;
		parameters.Sampler = TStaticSamplerState<SF_Bilinear>::GetRHI();

		parameters.InputOffsetAndScale = InputOffsetAndScale;
		parameters.CircleLongitude = CircleLongitude;
		parameters.MinLatitude = MinLatitude;
		parameters.LatitudeAngleDelta = LatitudeAngleDelta;
		parameters.LongitudeArc = LongitudeArc;
		parameters.LatitudeLength = LatitudeLength;

		auto OutputNormalMapRHIRef = GPUComputingUtils::GetRHITexture2D(Output);
		RHICmdList.Transition(FRHITransitionInfo(OutputNormalMapRHIRef, ERHIAccess::Unknown, ERHIAccess::UAVCompute));

		auto* OutputTexture = Cast<UTextureRenderTarget2D>(Output->GetNativeTexture());
		parameters.Output = static_cast<FTextureRenderTarget2DResource*>(OutputTexture->GetRenderTargetResource())->GetUnorderedAccessViewRHI();

		{
			auto ComputeShader = GPUComputingUtils::GetShader<FNormalMapGeneratorGlobalViewModeCS>();

			const uint32 ThreadGroupCountValueX = OutputNormalMapRHIRef->GetSizeX() / FNormalMapGeneratorGlobalViewModeCS::THREADGROUP_SIZE_X;
			const uint32 ThreadGroupCountValueY =
				FMath::DivideAndRoundUp(OutputNormalMapRHIRef->GetSizeY(), FNormalMapGeneratorGlobalViewModeCS::THREADGROUP_SIZE_Y);

			FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, parameters, FIntVector(ThreadGroupCountValueX, ThreadGroupCountValueY, 1));
		}

		RHICmdList.Transition(FRHITransitionInfo(OutputNormalMapRHIRef, ERHIAccess::UAVCompute, ERHIAccess::SRVGraphics));
	});
}

void FComputeNormalMapGenerator::DrawLocalViewMode(const TSharedPtr<GPUResources::FGPUResourceTexture2D>& InputElevation,
												   const FVector4f& InputOffsetAndScale,
												   float LatitudeLength,
												   float LongitudeLength,
												   const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Output)
{
	ENQUEUE_RENDER_COMMAND(GenerateNormalMapLocal)
	([InputElevation, InputOffsetAndScale, LongitudeLength, LatitudeLength, Output](FRHICommandListImmediate& RHICmdList) {
		FNormalMapGeneratorLocalViewModeCS::FParameters parameters;

		auto TextureRHIRef = GPUComputingUtils::GetRHITexture(InputElevation);
		RHICmdList.Transition(FRHITransitionInfo(TextureRHIRef, ERHIAccess::Unknown, ERHIAccess::SRVCompute));
		parameters.Input = TextureRHIRef;
		parameters.Sampler = TStaticSamplerState<SF_Bilinear>::GetRHI();

		parameters.InputOffsetAndScale = InputOffsetAndScale;
		parameters.LatitudeLength = LatitudeLength;
		parameters.LongitudeLength = LongitudeLength;

		auto OutputNormalMapRHIRef = GPUComputingUtils::GetRHITexture2D(Output);
		RHICmdList.Transition(FRHITransitionInfo(OutputNormalMapRHIRef, ERHIAccess::Unknown, ERHIAccess::UAVCompute));

		auto* OutputTexture = Cast<UTextureRenderTarget2D>(Output->GetNativeTexture());
		parameters.Output = static_cast<FTextureRenderTarget2DResource*>(OutputTexture->GetRenderTargetResource())->GetUnorderedAccessViewRHI();

		{
			auto ComputeShader = GPUComputingUtils::GetShader<FNormalMapGeneratorLocalViewModeCS>();

			const uint32 ThreadGroupCountValueX = OutputNormalMapRHIRef->GetSizeX() / FNormalMapGeneratorGlobalViewModeCS::THREADGROUP_SIZE_X;
			const uint32 ThreadGroupCountValueY =
				FMath::DivideAndRoundUp(OutputNormalMapRHIRef->GetSizeY(), FNormalMapGeneratorGlobalViewModeCS::THREADGROUP_SIZE_Y);

			FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, parameters, FIntVector(ThreadGroupCountValueX, ThreadGroupCountValueY, 1));
		}

		RHICmdList.Transition(FRHITransitionInfo(OutputNormalMapRHIRef, ERHIAccess::UAVCompute, ERHIAccess::SRVGraphics));
	});
}

} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
