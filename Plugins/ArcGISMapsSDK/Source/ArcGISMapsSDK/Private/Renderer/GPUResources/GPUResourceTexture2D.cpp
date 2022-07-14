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
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceTexture2D.h"

#include "Engine/Texture2D.h"
#include "RenderUtils.h"

namespace
{
uint32 getPixelSize(EPixelFormat Format)
{
	switch (Format)
	{
		case PF_A32B32G32R32F:
			return 16;
		case PF_A16B16G16R16:
			return 8;
		case PF_R32_FLOAT:
		case PF_B8G8R8A8:
		default:
			return 4;
	}
}

#define ALIGN_BLOCK(x, y) (((x) + y - 1) / y) * y
uint32 CalculatePitch(FRHITexture2D* Texture)
{
	const auto format = Texture->GetFormat();
	const auto width = Texture->GetSizeX();

	switch (format)
	{
		// See https://docs.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide for DXT formats
		case PF_DXT1:
			return std::max(1u, (width + 3) / 4) * 8;
		case PF_DXT3:
		case PF_DXT5:
			return std::max(1u, (width + 3) / 4) * 16;
		default:
			return ALIGN_BLOCK(width * getPixelSize(format), 4);
	}
}
} // namespace

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUResources
{
FGPUResourceTexture2D::FGPUResourceTexture2D(uint32 Width, uint32 Height, EPixelFormat Format, uint8 IsSRGB)
{
	auto Texture = UTexture2D::CreateTransient(Width, Height, Format);

	Texture->AddressX = TA_Clamp;
	Texture->AddressY = TA_Clamp;
	Texture->SRGB = IsSRGB;

	Texture->UpdateResource();

	Texture->AddToRoot();

	NativeTexture = Texture;
}

void FGPUResourceTexture2D::SetPixelData(const uint8* Data)
{
	TWeakPtr<FGPUResourceTexture2D, ESPMode::ThreadSafe> WeakThis_ = SharedThis(this);

	ENQUEUE_RENDER_COMMAND(UpdateTextureRegionsData)
	([WeakThis_, Data](FRHICommandListImmediate& RHICmdList) {
		if (auto StrongThis = WeakThis_.Pin())
		{
			auto texture2DRHI = static_cast<FTextureResource*>(StrongThis->NativeTexture->GetResource())->GetTexture2DRHI();

			auto height = FMath::DivideAndRoundUp(texture2DRHI->GetSizeY(), (uint32)GPixelFormats[texture2DRHI->GetFormat()].BlockSizeY);
			auto pitch = CalculatePitch(texture2DRHI);

			uint32 DestPitch;
			uint8* DestinationBuffer = static_cast<uint8*>(RHICmdList.LockTexture2D(texture2DRHI, 0, RLM_WriteOnly, DestPitch, false));
			{
				check(DestPitch >= pitch);

				if (DestPitch == pitch)
				{
					FMemory::Memcpy(DestinationBuffer, Data, height * pitch);
				}
				else
				{
					for (uint32 i = 0; i < height; ++i)
					{
						FMemory::Memcpy(DestinationBuffer + i * DestPitch, Data + i * pitch, FMath::Min(pitch, DestPitch));
					}
				}
			}
			RHICmdList.UnlockTexture2D(texture2DRHI, 0, false);
		}
	});
}
} // namespace GPUResources
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
