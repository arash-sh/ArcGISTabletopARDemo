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
#include "ArcGISMapsSDK/Renderer/GPUComputing/ImageComposerShaders.h"

#include "RHIStaticStates.h"
#include "ShaderParameterUtils.h"

// Compute shader

IMPLEMENT_SHADER_TYPE(, FImageComposerCS, TEXT("/Plugin/ArcGISMapsSDK/Private/ImageComposerCS.usf"), TEXT("ImageComposerCS"), SF_Compute);

const uint32 FImageComposerCS::THREADGROUP_SIZE_X = 8;
const uint32 FImageComposerCS::THREADGROUP_SIZE_Y = 8;

// Vertex shader

IMPLEMENT_SHADER_TYPE(, FImageComposerVS, TEXT("/Plugin/ArcGISMapsSDK/Private/ImageComposerPS.usf"), TEXT("ImageComposerVS"), SF_Vertex);

FImageComposerVS::FImageComposerVS() = default;

FImageComposerVS::FImageComposerVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
{
}

// Pixel shader

IMPLEMENT_SHADER_TYPE(, FImageComposerPS, TEXT("/Plugin/ArcGISMapsSDK/Private/ImageComposerPS.usf"), TEXT("ImageComposerPS"), SF_Pixel);

TGlobalResource<FImageComposerPSResources> GImageComposerPSResources;
