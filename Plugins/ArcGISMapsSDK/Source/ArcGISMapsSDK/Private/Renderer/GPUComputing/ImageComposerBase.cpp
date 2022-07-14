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
#include "ArcGISMapsSDK/Renderer/GPUComputing/ImageComposerBase.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
namespace
{
uint32 CalculateNumIterations(uint32 count)
{
	if (count <= 8)
	{
		return 1;
	}

	return 1 + (count - 8) / 7 + ((count - 8) % 7 == 0 ? 0 : 1);
}
} // namespace

FImageComposerBase::FImageComposerBase()
{
	TempRenderTarget = ::MakeShared<GPUResources::FGPUResourceRenderTarget>(256, 256, PF_R8G8B8A8, false);
}

FImageComposerBase::~FImageComposerBase() = default;

void FImageComposerBase::Compose(const std::vector<FComposableImage>& InputImages,
								 const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Output)
{
	uint32 ProcessedTextures = 0;
	uint32 RemainingTextures = InputImages.size();
	bool RequiresAuxTexture = RemainingTextures > 8;
	TSharedPtr<const GPUResources::FGPUResourceRenderTarget> NextAuxTexture = TempRenderTarget;
	TSharedPtr<const GPUResources::FGPUResourceRenderTarget> NextOutputTexture = Output;

	if (CalculateNumIterations(RemainingTextures) % 2 == 0)
	{
		std::swap(NextAuxTexture, NextOutputTexture);
	}

	while (RemainingTextures > 0)
	{
		uint32 TexturesToUse = std::min(RemainingTextures, 8u);

		std::vector<TSharedPtr<const GPUResources::FGPUResourceTexture>> Inputs;
		std::vector<FVector4f> OffsetScaleOpacities;

		if (RequiresAuxTexture && ProcessedTextures >= 8)
		{
			Inputs.push_back(NextAuxTexture);
			OffsetScaleOpacities.push_back(FVector4f(0, 0, 1, 1));

			TexturesToUse = std::min(RemainingTextures, 7u);
		}

		for (uint32 i = 0; i < TexturesToUse; ++i)
		{
			FVector4f OffsetScaleOpacity = InputImages[ProcessedTextures].Extent;

			OffsetScaleOpacity.W = InputImages[ProcessedTextures].Opacity;

			Inputs.push_back(InputImages[ProcessedTextures].Image);
			OffsetScaleOpacities.push_back(OffsetScaleOpacity);

			ProcessedTextures++;
		}

		Draw(std::move(Inputs), std::move(OffsetScaleOpacities), NextOutputTexture);

		RemainingTextures -= TexturesToUse;

		std::swap(NextAuxTexture, NextOutputTexture);
	}

	// This must be done here to make sure mip maps are generated.
	Output->GetNativeRenderTarget2D()->UpdateResourceImmediate(false);
}

void FImageComposerBase::Compose(const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& SourceA,
								 const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& SourceB,
								 float Alpha,
								 const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Output,
								 const FVector4f& Extent)
{
	FVector4f OffsetScaleOpacity1 = Extent;
	FVector4f OffsetScaleOpacity2 = Extent;

	OffsetScaleOpacity1.W = 1;
	OffsetScaleOpacity2.W = Alpha;

	// clang-format off
	Draw
	(
		{SourceA, SourceB},
		{OffsetScaleOpacity1, OffsetScaleOpacity2},
		Output
	);
	// clang-format on
}

void FImageComposerBase::Copy(const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Source,
							  const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Output,
							  const FVector4f& Extent)
{
	FVector4f OffsetScaleOpacity = Extent;

	OffsetScaleOpacity.W = 1;

	// clang-format off
	Draw
	(
		{Source},
		{OffsetScaleOpacity},
		Output
	);
	// clang-format on
}
} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
