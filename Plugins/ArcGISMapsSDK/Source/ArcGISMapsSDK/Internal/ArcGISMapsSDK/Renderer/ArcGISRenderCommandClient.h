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

#include "CoreMinimal.h"

#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISRenderCommand.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISRenderCommandServer.h"
#include "ArcGISMapsSDK/Renderer/GPUComputing/IImageComposer.h"
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceProvider.h"
#include "ArcGISMapsSDK/Renderer/SceneComponents/SceneComponentProvider.h"

class UArcGISMapComponent;

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
class INormalMapGenerator;
} // namespace GPUComputing

class FArcGISRenderCommandClient
{
public:
	FArcGISRenderCommandClient(UObject* InOuter,
							   TSharedPtr<GPUResources::FGPUResourceProvider> InGPUResourceProvider,
							   TSharedPtr<SceneComponents::FSceneComponentProvider> InSceneComponentProvider,
							   TSharedPtr<GPUComputing::IImageComposer> InImageComposer,
							   TSharedPtr<GPUComputing::INormalMapGenerator> InNormalMapGenerator);
	void ExecuteRenderCommand(const GameEngine::RenderCommandQueue::ArcGISRenderCommand& renderCommand);

private:
	TSharedPtr<GPUResources::FGPUResourceProvider> GPUResourceProvider;
	TSharedPtr<GPUComputing::IImageComposer> ImageComposer;
	UArcGISMapComponent* MapComponent{nullptr};
	TSharedPtr<GPUComputing::INormalMapGenerator> NormalMapGenerator;
	GameEngine::RenderCommandQueue::ArcGISRenderCommandServer RenderCommandServer;
	TSharedPtr<SceneComponents::FSceneComponentProvider> SceneComponentProvider;
};
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
