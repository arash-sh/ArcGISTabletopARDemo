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
#include "ArcGISMapsSDK/Renderer/ArcGISRenderCommandClient.h"

#include "Misc/EngineVersionComparison.h"

#include "ArcGISMapsSDK/API/GameEngine/Math/ArcGISVector4.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISDataBufferView.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISRenderCommandType.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISComposeCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISComposedTextureElement.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISCopyCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISCreateMaterialCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISCreateRenderTargetCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISCreateSceneComponentCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISCreateTextureCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISDestroyMaterialCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISDestroyRenderTargetCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISDestroySceneComponentCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISDestroyTextureCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISGenerateMipmapsCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISGenerateNormalTextureCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISMaterialType.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISMeshBufferChangeType.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISMultipleComposeCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMaterialCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMaterialNamedTexturePropertyCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMaterialRenderTargetPropertyCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMaterialScalarPropertyCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMaterialTexturePropertyCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMaterialVectorPropertyCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMeshBufferCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMeshCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetSceneComponentPivotCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetTexturePixelDataCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetVisibleCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISTextureFormat.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISDataBuffer.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/INormalMapGenerator.h"
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceMaterial.h"

namespace
{
template <typename T>
TArrayView<T> ToArrayView(const Esri::GameEngine::RenderCommandQueue::ArcGISDataBufferView& View)
{
	return TArrayView<T>(reinterpret_cast<T*>(View.Data), View.Size / sizeof(T));
}

template <typename To, typename From>
To TransformStruct(const From& from)
{
	To to;

	static_assert(sizeof(From) == sizeof(To), "The input struct size should be the same as the output one");
	static_assert(std::is_trivially_copyable<From>::value, "The input struct should be trivially copyable");
	static_assert(std::is_trivially_copyable<To>::value, "The output struct should be trivially copyable");

	memcpy(&to, &from, sizeof(To));

	return to;
}

ObbTransform TransformObbForENU(UArcGISMapComponent* InMapComponent, const ObbTransform& InObbTransform)
{
	const auto& ENUToWorld = InMapComponent->GetENUToWorldTransformAtOrigin();

	const auto& pos = InObbTransform.pos;
	const auto& scale = InObbTransform.scale;
	const auto& rot = InObbTransform.rot;

	ObbTransform result;
	// Obb is in UE coordinates, convert, transform, convert back (with scale)
	result.pos = InMapComponent->ToEnginePosition(pos);
	result.rot = ENUToWorld.GetRotation() * rot;
	result.scale = (scale * 100.).GetAbs();

	return result;
}
} // namespace

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
FArcGISRenderCommandClient::FArcGISRenderCommandClient(UObject* InOuter,
													   TSharedPtr<GPUResources::FGPUResourceProvider> InGPUResourceProvider,
													   TSharedPtr<SceneComponents::FSceneComponentProvider> InSceneComponentProvider,
													   TSharedPtr<GPUComputing::IImageComposer> InImageComposer,
													   TSharedPtr<GPUComputing::INormalMapGenerator> InNormalMapGenerator)
: GPUResourceProvider(InGPUResourceProvider),
  ImageComposer(InImageComposer),
  NormalMapGenerator(InNormalMapGenerator),
  SceneComponentProvider(InSceneComponentProvider)
{
	MapComponent = UArcGISMapComponent::GetMapComponent(InOuter);
	check(MapComponent != NULL);
}

void FArcGISRenderCommandClient::ExecuteRenderCommand(const GameEngine::RenderCommandQueue::ArcGISRenderCommand& RenderCommand)
{
	using namespace GameEngine::RenderCommandQueue;

	switch (RenderCommand.GetType())
	{
		case ArcGISRenderCommandType::CreateMaterial:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISCreateMaterialCommandParameters>();

			GPUResourceProvider->CreateMaterial(Params.MaterialId, Params.MaterialType, Params.Material);

			break;
		}

		case ArcGISRenderCommandType::CreateRenderTarget:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISCreateRenderTargetCommandParameters>();

			GPUResourceProvider->CreateRenderTarget(Params.RenderTargetId, Params.Width, Params.Height, Params.TextureFormat, Params.HasMipMaps);

			break;
		}

		case ArcGISRenderCommandType::CreateTexture:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISCreateTextureCommandParameters>();

			GPUResourceProvider->CreateTexture(Params.TextureId, Params.Width, Params.Height, Params.TextureFormat, Params.IsSRGB);

			break;
		}

		case ArcGISRenderCommandType::CreateSceneComponent:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISCreateSceneComponentCommandParameters>();

			SceneComponentProvider->CreateSceneComponent(Params.SceneComponentId, Params.LayerId);

			break;
		}

		case ArcGISRenderCommandType::DestroyMaterial:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISDestroyMaterialCommandParameters>();

			GPUResourceProvider->DestroyMaterial(Params.MaterialId);
			break;
		}

		case ArcGISRenderCommandType::DestroyRenderTarget:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISDestroyRenderTargetCommandParameters>();

			GPUResourceProvider->DestroyRenderTarget(Params.RenderTargetId);
			break;
		}

		case ArcGISRenderCommandType::DestroyTexture:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISDestroyTextureCommandParameters>();

			GPUResourceProvider->DestroyTexture(Params.TextureId);
			break;
		}

		case ArcGISRenderCommandType::DestroySceneComponent:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISDestroySceneComponentCommandParameters>();

			SceneComponentProvider->DestroySceneComponent(Params.SceneComponentId);

			break;
		}

		case ArcGISRenderCommandType::MultipleCompose:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISMultipleComposeCommandParameters>();

			const auto& Target = GPUResourceProvider->GetRenderTarget(Params.TargetId);

			std::vector<GPUComputing::FComposableImage> Images;

			auto Texture = reinterpret_cast<Parameters::ArcGISComposedTextureElement*>(Params.ComposedTextures.Data);
			auto Texture_end =
				reinterpret_cast<Parameters::ArcGISComposedTextureElement*>(Params.ComposedTextures.Data + Params.ComposedTextures.Size);

			while (Texture < Texture_end)
			{
				auto TargetTexture = GPUResourceProvider->GetTexture(Texture->TextureId);

				Images.emplace_back(GPUComputing::FComposableImage{TransformStruct<FVector4f>(Texture->Region), TargetTexture, Texture->Opacity});
				Texture++;
			}

			ImageComposer->Compose(Images, Target);
			break;
		}

		case ArcGISRenderCommandType::Compose:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISComposeCommandParameters>();

			const auto& Source1 = GPUResourceProvider->GetRenderTarget(Params.SourceId1);
			const auto& Source2 = GPUResourceProvider->GetRenderTarget(Params.SourceId2);
			const auto& Target = GPUResourceProvider->GetRenderTarget(Params.TargetId);

			ImageComposer->Compose(Source1, Source2, Params.Alpha, Target, TransformStruct<FVector4f>(Params.Region));
			break;
		}

		case ArcGISRenderCommandType::Copy:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISCopyCommandParameters>();

			const auto& Source = GPUResourceProvider->GetRenderTarget(Params.SourceId);
			const auto& Target = GPUResourceProvider->GetRenderTarget(Params.TargetId);

			ImageComposer->Copy(Source, Target, TransformStruct<FVector4f>(Params.Region));
			break;
		}

		case ArcGISRenderCommandType::GenerateNormalTexture:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISGenerateNormalTextureCommandParameters>();
			const auto& Target = GPUResourceProvider->GetRenderTarget(Params.TargetId);
			const auto& Elevation =
				Params.ElevationId != 0 ? GPUResourceProvider->GetTexture(Params.ElevationId) : GPUResourceProvider->GetBlackFloatTexture();
			auto TileExtent = TransformStruct<FVector4f>(Params.TileExtent);
			auto Region = TransformStruct<FVector4f>(Params.TextureExtent);
			NormalMapGenerator->Compute(Elevation, TileExtent, Region, Target);

			break;
		}

		case ArcGISRenderCommandType::SetTexturePixelData:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetTexturePixelDataCommandParameters>();
			const auto& Texture = GPUResourceProvider->GetTexture(Params.TextureId);

			Texture->SetPixelData(Params.Pixels.Data);

			break;
		}

		case ArcGISRenderCommandType::SetMaterialScalarProperty:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMaterialScalarPropertyCommandParameters>();
			static auto Names = std::array<FString, 5>{"ClippingMode", "UseUVIndex", "BlendFactor", "PositionsBlendFactor", "Opacity"};

			const auto& Material = GPUResourceProvider->GetMaterial(Params.MaterialId);
			const auto& Name = Names[static_cast<int>(Params.MaterialScalarProperty)];

			Material->SetFloat(Name, Params.Value);
			break;
		}

		case ArcGISRenderCommandType::SetMaterialVectorProperty:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMaterialVectorPropertyCommandParameters>();

			static auto Names = std::array<FString, 2>{"MapAreaMin", "MapAreaMax"};

			const auto& Material = GPUResourceProvider->GetMaterial(Params.MaterialId);
			const auto& Name = Names[static_cast<int>(Params.MaterialVectorProperty)];
			const auto Value = TransformStruct<FVector4f>(Params.Value);

			Material->SetVector(Name, Value);
			break;
		}

		case ArcGISRenderCommandType::SetMaterialRenderTargetProperty:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMaterialRenderTargetPropertyCommandParameters>();

			static auto Names = std::array<FString, 4>{"ImageryMap0", "ImageryMap1", "NormalMap0", "NormalMap1"};

			const auto& Material = GPUResourceProvider->GetMaterial(Params.MaterialId);
			const auto& RenderTarget = GPUResourceProvider->GetRenderTarget(Params.Value);
			const auto& Name = Names[static_cast<int>(Params.MaterialRenderTargetProperty)];

			Material->SetTexture(Name, RenderTarget);
			break;
		}

		case ArcGISRenderCommandType::SetMaterialTextureProperty:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMaterialTexturePropertyCommandParameters>();

			static auto Names = std::array<FString, 5>{"BaseMap", "UVRegionLUT", "PositionsMap0", "PositionsMap1", "FeatureIds"};

			const auto& Material = GPUResourceProvider->GetMaterial(Params.MaterialId);
			const auto& Texture = GPUResourceProvider->GetTexture(Params.Value);
			const auto& Name = Names[static_cast<int>(Params.MaterialTextureProperty)];

			Material->SetTexture(Name, Texture);
			break;
		}

		case ArcGISRenderCommandType::SetMaterialNamedTextureProperty:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMaterialNamedTexturePropertyCommandParameters>();

			const auto& Material = GPUResourceProvider->GetMaterial(Params.MaterialId);
			const auto& Texture = GPUResourceProvider->GetTexture(Params.Value);
			const FString Name{static_cast<int32>(Params.TextureName.Size), reinterpret_cast<const char*>(Params.TextureName.Data)};

			Material->SetTexture(Name, Texture);
			break;
		}
		case ArcGISRenderCommandType::GenerateMipMaps:
		{
			RenderCommand.GetParameters<Parameters::ArcGISGenerateMipMapsCommandParameters>();
			break;
		}

		case ArcGISRenderCommandType::SetVisible:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetVisibleCommandParameters>();

			const auto& SceneComponent = SceneComponentProvider->GetSceneComponent(Params.SceneComponentId);

			SceneComponent->SetVisible(Params.IsVisible);
			break;
		}

		case ArcGISRenderCommandType::SetMaterial:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMaterialCommandParameters>();

			const auto& SceneComponent = SceneComponentProvider->GetSceneComponent(Params.SceneComponentId);
			const auto& Material = GPUResourceProvider->GetMaterial(Params.MaterialId);

			const bool MaskTerrain = Params.MaskTerrain;
			SceneComponent->SetMasksTerrain(MaskTerrain);

			if (MaskTerrain)
			{
				const auto& Obb = Params.OrientedBoundingBox;
				FVector3d ObbPos(Obb.CenterX, Obb.CenterY, Obb.CenterZ);
				FVector3d ObbScale(Obb.ExtentX, Obb.ExtentY, Obb.ExtentZ);
				FQuat ObbRot(Obb.OrientationX, Obb.OrientationY, Obb.OrientationZ, Obb.OrientationW);
				// Scale is doubled to go from half-size to size
				SceneComponent->SetObbTransform(TransformObbForENU(MapComponent, ObbTransform{ObbPos, 2. * ObbScale, ObbRot}));
			}

			SceneComponent->SetMaterial(Material);
			break;
		}

		case ArcGISRenderCommandType::SetMesh:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMeshCommandParameters>();
			const auto& SceneComponent = SceneComponentProvider->GetSceneComponent(Params.SceneComponentId);

			auto SafeMeshData = ::MakeShared<FGPUResourceMesh>();

			// Assure the vertex attributes are always packed into the same UV index
			static constexpr auto RealUVIndex = 0;
			static constexpr auto FeatureIndicesUvIndex = 1;
			static constexpr auto UvRegionIdsUvIndex = 2;

			bool HasFeatureIndices = Params.FeatureIndices.Size > 0u;
			bool HasUvRegionIds = Params.UvRegionIds.Size > 0u;

			auto NumUVs = 1 + std::max(HasFeatureIndices * FeatureIndicesUvIndex, HasUvRegionIds * UvRegionIdsUvIndex);

			SafeMeshData->UVs.SetNum(NumUVs);

			if (HasFeatureIndices)
			{
				SafeMeshData->bUseFullPrecisionUVs = true;
				SafeMeshData->UVs[FeatureIndicesUvIndex] = {ToArrayView<float>(Params.FeatureIndices), 2};
			}

			if (HasUvRegionIds)
			{
				SafeMeshData->UVs[UvRegionIdsUvIndex] = {ToArrayView<float>(Params.UvRegionIds), 1};
			}

			SafeMeshData->Normals = ToArrayView<FVector3f>(Params.Normals);
			SafeMeshData->Tangents = ToArrayView<FVector4f>(Params.Tangents);
			SafeMeshData->UVs[RealUVIndex] = {ToArrayView<float>(Params.Uvs), 2};
			SafeMeshData->Triangles = ToArrayView<int32>(Params.Triangles);
			SafeMeshData->Colors = ToArrayView<uint32>(Params.Colors);
			SafeMeshData->Positions = ToArrayView<FVector3f>(Params.Positions);

			SceneComponent->SetMesh(SafeMeshData);
			break;
		}

		case ArcGISRenderCommandType::SetSceneComponentPivot:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetSceneComponentPivotCommandParameters>();

			const auto& SceneComponent = SceneComponentProvider->GetSceneComponent(Params.SceneComponentId);

			SceneComponent->SetScale(FVector3d(100.0));

			auto Transform = MapComponent->GetENUToWorldTransformAtOrigin();
			SceneComponent->SetRotation(Transform.GetRotation());

			// Pivots and vertices are in UE coordinates.
			auto UEFrameCoordinates = Params;

			// To convert from UE into ECEF/Projected, we have to invert the Y coordinate because of left-handed UE Frame.
			FVector3d Coordinates(UEFrameCoordinates.X, UEFrameCoordinates.Y, UEFrameCoordinates.Z);

			auto EngineLocation = MapComponent->ToEnginePosition(Coordinates);

			SceneComponent->SetLocation(EngineLocation);

			break;
		}

		case ArcGISRenderCommandType::CommandGroupBegin:
		case ArcGISRenderCommandType::CommandGroupEnd:
		case ArcGISRenderCommandType::SetMeshBuffer:
			break;

		default:
			throw "Unknown render command type!";
	}
}
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
