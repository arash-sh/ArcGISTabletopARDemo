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
#include "ArcGISMapsSDK/Renderer/GPUComputing/GPUComputingUtils.h"
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceRenderTarget.h"
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceTexture2D.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
FTexture2DRHIRef GPUComputingUtils::GetRHITexture2D(const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Texture)
{
	auto* NativeTexture = Texture->GetNativeTexture();
	FTexture2DRHIRef OutputRHIRef = (NativeTexture && NativeTexture->GetResource() && NativeTexture->GetResource()->TextureRHI) ?
										NativeTexture->GetResource()->TextureRHI->GetTexture2D() :
										nullptr;
	verify(OutputRHIRef);
	return OutputRHIRef;
}

FTextureRHIRef GPUComputingUtils::GetRHITexture(const TSharedPtr<GPUResources::FGPUResourceTexture2D>& Texture)
{
	auto* NativeTexture = Texture->GetNativeTexture();
	auto TextureRHIRef = NativeTexture && NativeTexture->GetResource() ? NativeTexture->GetResource()->TextureRHI : nullptr;
	verify(TextureRHIRef);
	return TextureRHIRef;
}

} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
