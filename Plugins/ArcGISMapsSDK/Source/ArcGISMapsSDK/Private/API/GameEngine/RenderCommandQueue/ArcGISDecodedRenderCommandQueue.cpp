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
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISDecodedRenderCommandQueue.h"

#include "ArcGISMapsSDK/API/GameEngine/Math/ArcGISVector4.h"
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
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISNullCommandParameters.h"
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
#include "ArcGISMapsSDK/API/Unreal/ArcGISDataBuffer.h"

namespace Esri
{
namespace GameEngine
{
namespace RenderCommandQueue
{
ArcGISDecodedRenderCommandQueue::ArcGISDecodedRenderCommandQueue(Unreal::ArcGISDataBuffer<uint8_t> InRawRenderCommands)
{
	RawRenderCommands = std::move(InRawRenderCommands);
}

#define DECODE_COMMAND_WITH_PARAMETERS(EnumValue, ParameterName)                 \
	case ArcGISRenderCommandType::EnumValue:                                     \
	{                                                                            \
		return GetRenderCommand<Parameters::ParameterName##CommandParameters>(); \
	}

#define DECODE_COMMAND(EnumValue) DECODE_COMMAND_WITH_PARAMETERS(EnumValue, ArcGIS##EnumValue)

ArcGISRenderCommand* ArcGISDecodedRenderCommandQueue::GetNextCommand()
{
	if (RawRenderCommands && CurrentOffset < RawRenderCommands.GetSizeBytes())
	{
		auto CommandType = GetCommandType();
		switch (CommandType)
		{
			DECODE_COMMAND(Compose);
			DECODE_COMMAND(Copy);
			DECODE_COMMAND(CreateMaterial);
			DECODE_COMMAND(CreateRenderTarget);
			DECODE_COMMAND(CreateSceneComponent);
			DECODE_COMMAND(CreateTexture);
			DECODE_COMMAND(DestroyMaterial);
			DECODE_COMMAND(DestroyRenderTarget);
			DECODE_COMMAND(DestroySceneComponent);
			DECODE_COMMAND(DestroyTexture);
			DECODE_COMMAND(GenerateMipMaps);
			DECODE_COMMAND(GenerateNormalTexture);
			DECODE_COMMAND(MultipleCompose);
			DECODE_COMMAND(SetMaterial);
			DECODE_COMMAND(SetMaterialNamedTextureProperty);
			DECODE_COMMAND(SetMaterialRenderTargetProperty);
			DECODE_COMMAND(SetMaterialScalarProperty);
			DECODE_COMMAND(SetMaterialTextureProperty);
			DECODE_COMMAND(SetMaterialVectorProperty);
			DECODE_COMMAND(SetMesh);
			DECODE_COMMAND(SetMeshBuffer);
			DECODE_COMMAND(SetSceneComponentPivot);
			DECODE_COMMAND(SetTexturePixelData);
			DECODE_COMMAND(SetVisible);
			DECODE_COMMAND_WITH_PARAMETERS(CommandGroupBegin, ArcGISNull);
			DECODE_COMMAND_WITH_PARAMETERS(CommandGroupEnd, ArcGISNull);
			default:
			{
				// debug ignore unknown commands
				throw "Unknown render command type!";
			}
		}
	}
	return nullptr;
}

ArcGISRenderCommandType ArcGISDecodedRenderCommandQueue::GetCommandType()
{
	return *reinterpret_cast<ArcGISRenderCommandType*>(RawRenderCommands.GetData<uint8>() + CurrentOffset);
}

template <typename T>
ArcGISRenderCommand* ArcGISDecodedRenderCommandQueue::GetRenderCommand()
{
	auto Address = RawRenderCommands.GetData<uint8>() + CurrentOffset;
	CurrentOffset += sizeof(ArcGISRenderCommandType) + sizeof(T);
	return reinterpret_cast<ArcGISRenderCommand*>(Address);
}
} // namespace RenderCommandQueue
} // namespace GameEngine
} // namespace Esri
