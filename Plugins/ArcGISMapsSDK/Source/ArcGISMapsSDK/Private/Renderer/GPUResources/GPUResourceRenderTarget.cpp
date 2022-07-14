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
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceRenderTarget.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUResources
{
FGPUResourceRenderTarget::FGPUResourceRenderTarget(uint32 Width, uint32 Height, EPixelFormat Format, bool bUseMipmaps)
{
	auto RenderTarget = NewObject<UTextureRenderTarget2D>();

#if PLATFORM_ANDROID || PLATFORM_APPLE
	RenderTarget->bAutoGenerateMips = false;
#else
	RenderTarget->bAutoGenerateMips = bUseMipmaps;
#endif
	RenderTarget->bCanCreateUAV = true;
	RenderTarget->bNeedsTwoCopies = false;
	RenderTarget->ClearColor = FLinearColor::White;

	RenderTarget->InitCustomFormat(Width, Height, Format, true);
	RenderTarget->UpdateResourceImmediate(true);

	RenderTarget->AddToRoot();

	NativeTexture = RenderTarget;
}

FGPUResourceRenderTarget::FGPUResourceRenderTarget(UTextureRenderTarget2D* InNativeRenderTarget)
{
	InNativeRenderTarget->AddToRoot();
	NativeTexture = InNativeRenderTarget;
}

UTextureRenderTarget2D* FGPUResourceRenderTarget::GetNativeRenderTarget2D() const
{
	return Cast<UTextureRenderTarget2D>(NativeTexture);
}

uint32 FGPUResourceRenderTarget::GetHeight() const
{
	return GetNativeRenderTarget2D()->SizeY;
}

uint32 FGPUResourceRenderTarget::GetWidth() const
{
	return GetNativeRenderTarget2D()->SizeX;
}

} // namespace GPUResources
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
