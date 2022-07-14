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
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceProvider.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"

#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISMaterialType.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISTextureFormat.h"

namespace
{
EPixelFormat ConvertToEPixelFormat(Esri::GameEngine::RenderCommandQueue::Parameters::ArcGISTextureFormat Format)
{
	using namespace Esri::GameEngine::RenderCommandQueue::Parameters;
	switch (Format)
	{
		case ArcGISTextureFormat::RGBA16UNorm:
			return PF_A16B16G16R16;
		case ArcGISTextureFormat::R32Norm:
#ifdef PLATFORM_APPLE
			return PF_R32_FLOAT;
#else
			return PF_R32_SINT;
#endif
		case ArcGISTextureFormat::RGBA32Float:
			return PF_A32B32G32R32F;
		case ArcGISTextureFormat::RGBA8UNorm:
			return PF_R8G8B8A8;
		case ArcGISTextureFormat::BGRA8UNorm:
			return PF_B8G8R8A8;
		case ArcGISTextureFormat::R32Float:
			return PF_R32_FLOAT;
		case ArcGISTextureFormat::DXT1:
			return PF_DXT1;
		case ArcGISTextureFormat::DXT3:
			return PF_DXT3;
		case ArcGISTextureFormat::DXT5:
			return PF_DXT5;
		case ArcGISTextureFormat::ETC2RGB8:
		case ArcGISTextureFormat::ETC2SRGB8:
			return PF_ETC2_RGB;
		case ArcGISTextureFormat::ETC2RGB8PunchthroughAlpha1:
		case ArcGISTextureFormat::ETC2SRGB8PunchthroughAlpha1:
		case ArcGISTextureFormat::ETC2EacSRGBA8:
		case ArcGISTextureFormat::ETC2EacRGBA8:
			return PF_ETC2_RGBA;
		default:
			throw "Unkown PixelFormat";
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
FGPUResourceProvider::FGPUResourceProvider(UObject* InOuter) : Outer(InOuter)
{
	// Create zero-valued float texture
	const float BlackFloatTextureData{0.0f};
	BlackFloatTexture = ::MakeShared<FGPUResourceTexture2D>(
		1, 1, ConvertToEPixelFormat(GameEngine::RenderCommandQueue::Parameters::ArcGISTextureFormat::R32Float), false);
	BlackFloatTexture->SetPixelData(reinterpret_cast<const uint8*>(&BlackFloatTextureData));
}

const TSharedPtr<FGPUResourceTexture2D>& FGPUResourceProvider::GetBlackFloatTexture() const
{
	return BlackFloatTexture;
}

TSharedPtr<FGPUResourceMaterial> FGPUResourceProvider::CreateMaterial(uint32 Id,
																	  GameEngine::RenderCommandQueue::Parameters::ArcGISMaterialType MaterialType,
																	  void* CustomMaterial)
{
	TSharedPtr<FGPUResourceMaterial> NewMaterial;

	if (CustomMaterial)
	{
		NewMaterial = ::MakeShared<FGPUResourceMaterial>(Cast<UMaterialInterface>(reinterpret_cast<FWeakObjectPtr*>(CustomMaterial)->Get()));
	}
	else
	{
		bool IsTileMaterial = MaterialType == GameEngine::RenderCommandQueue::Parameters::ArcGISMaterialType::Tile;
		auto Name = IsTileMaterial ? "Material'/ArcGISMapsSDK/Materials/Tile.Tile'" : "Material'/ArcGISMapsSDK/Materials/SceneNode.SceneNode'";

		NewMaterial = ::MakeShared<FGPUResourceMaterial>(LoadObject<UMaterial>(Outer, ANSI_TO_TCHAR(Name)));

		if (IsTileMaterial)
		{
			static const FString OcclusionRenderTargetName("OcclusionRenderTarget");
			NewMaterial->SetTexture(OcclusionRenderTargetName, OcclusionRenderTarget);
		}
	}

	Materials.Emplace(Id, NewMaterial);

	return NewMaterial;
}

TSharedPtr<FGPUResourceRenderTarget> FGPUResourceProvider::CreateRenderTarget(uint32 Id,
																			  uint32 Width,
																			  uint32 Height,
																			  GameEngine::RenderCommandQueue::Parameters::ArcGISTextureFormat Format,
																			  bool bUseMipmaps)
{
	auto NewRenderTarget = ::MakeShared<FGPUResourceRenderTarget>(Width, Height, ConvertToEPixelFormat(Format), bUseMipmaps);

	RenderTargets.Emplace(Id, NewRenderTarget);

	return NewRenderTarget;
}

TSharedPtr<FGPUResourceTexture2D> FGPUResourceProvider::CreateTexture(uint32 Id,
																	  uint32 Width,
																	  uint32 Height,
																	  GameEngine::RenderCommandQueue::Parameters::ArcGISTextureFormat Format,
																	  uint8 IsSRGB)
{
	auto NewTexture = ::MakeShared<FGPUResourceTexture2D>(Width, Height, ConvertToEPixelFormat(Format), IsSRGB);

	Textures.Emplace(Id, NewTexture);

	return NewTexture;
}

TSharedPtr<FGPUResourceMaterial> FGPUResourceProvider::GetMaterial(uint32 Id)
{
	return Materials[Id];
}

TSharedPtr<FGPUResourceRenderTarget> FGPUResourceProvider::GetRenderTarget(uint32 Id)
{
	return RenderTargets[Id];
}

TSharedPtr<FGPUResourceTexture2D> FGPUResourceProvider::GetTexture(uint32 Id)
{
	return Textures[Id];
}

void FGPUResourceProvider::DestroyMaterial(uint32 Id)
{
	Materials.Remove(Id);
}

void FGPUResourceProvider::DestroyRenderTarget(uint32 Id)
{
	RenderTargets.Remove(Id);
}

void FGPUResourceProvider::DestroyTexture(uint32 Id)
{
	Textures.Remove(Id);
}

void FGPUResourceProvider::SetOcclusionRenderTarget(UTextureRenderTarget2D* InOcclusionRenderTarget)
{
	OcclusionRenderTarget = ::MakeShared<FGPUResourceRenderTarget>(InOcclusionRenderTarget);
}
} // namespace GPUResources
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
