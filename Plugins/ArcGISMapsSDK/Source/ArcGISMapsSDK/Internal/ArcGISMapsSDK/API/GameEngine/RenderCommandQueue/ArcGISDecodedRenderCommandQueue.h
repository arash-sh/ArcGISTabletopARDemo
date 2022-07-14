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
#pragma once

#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISRenderCommand.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISRenderCommandType.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISDataBuffer.h"

namespace Esri
{
namespace GameEngine
{
namespace RenderCommandQueue
{
class ArcGISDecodedRenderCommandQueue
{
public:
	ArcGISDecodedRenderCommandQueue() = default;
	ArcGISDecodedRenderCommandQueue(Unreal::ArcGISDataBuffer<uint8_t> InRawRenderCommands);

	ArcGISRenderCommand* GetNextCommand();
	ArcGISRenderCommandType GetCommandType();

private:
	template <typename T>
	ArcGISRenderCommand* GetRenderCommand();

	Unreal::ArcGISDataBuffer<uint8_t> RawRenderCommands{};
	uint32 CurrentOffset{0};
};
} // namespace RenderCommandQueue
} // namespace GameEngine
} // namespace Esri
