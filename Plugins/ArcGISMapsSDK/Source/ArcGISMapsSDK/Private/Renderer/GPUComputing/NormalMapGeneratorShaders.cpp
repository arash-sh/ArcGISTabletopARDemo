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
#include "ArcGISMapsSDK/Renderer/GPUComputing/NormalMapGeneratorShaders.h"

#include "RHIStaticStates.h"
#include "ShaderParameterUtils.h"

// Compute shader - global view mode

IMPLEMENT_SHADER_TYPE(,
					  FNormalMapGeneratorGlobalViewModeCS,
					  TEXT("/Plugin/ArcGISMapsSDK/Private/NormalMapGeneratorGlobalViewModeCS.usf"),
					  TEXT("NormalMapGeneratorGlobalViewModeCS"),
					  SF_Compute);

const uint32 FNormalMapGeneratorGlobalViewModeCS::THREADGROUP_SIZE_X = 8;
const uint32 FNormalMapGeneratorGlobalViewModeCS::THREADGROUP_SIZE_Y = 8;

// Compute shader - local view mode

IMPLEMENT_SHADER_TYPE(,
					  FNormalMapGeneratorLocalViewModeCS,
					  TEXT("/Plugin/ArcGISMapsSDK/Private/NormalMapGeneratorLocalViewModeCS.usf"),
					  TEXT("NormalMapGeneratorLocalViewModeCS"),
					  SF_Compute);

const uint32 FNormalMapGeneratorLocalViewModeCS::THREADGROUP_SIZE_X = 8;
const uint32 FNormalMapGeneratorLocalViewModeCS::THREADGROUP_SIZE_Y = 8;

// Vertex shader

IMPLEMENT_SHADER_TYPE(,
					  FNormalMapGeneratorVS,
					  TEXT("/Plugin/ArcGISMapsSDK/Private/NormalMapGeneratorGlobalViewModePS.usf"),
					  TEXT("NormalMapGeneratorVS"),
					  SF_Vertex);

FNormalMapGeneratorVS::FNormalMapGeneratorVS() = default;

FNormalMapGeneratorVS::FNormalMapGeneratorVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FGlobalShader(Initializer)
{
}

// Pixel shader - global view mode

IMPLEMENT_SHADER_TYPE(,
					  FNormalMapGeneratorGlobalViewModePS,
					  TEXT("/Plugin/ArcGISMapsSDK/Private/NormalMapGeneratorGlobalViewModePS.usf"),
					  TEXT("NormalMapGeneratorGlobalViewModePS"),
					  SF_Pixel);

TGlobalResource<FNormalMapGeneratorGlobalViewModePSResources> GNormalMapGeneratorGlobalViewModePSResources;

// Pixel shader - local view mode

IMPLEMENT_SHADER_TYPE(,
					  FNormalMapGeneratorLocalViewModePS,
					  TEXT("/Plugin/ArcGISMapsSDK/Private/NormalMapGeneratorLocalViewModePS.usf"),
					  TEXT("NormalMapGeneratorLocalViewModePS"),
					  SF_Pixel);

TGlobalResource<FNormalMapGeneratorLocalViewModePSResources> GNormalMapGeneratorLocalViewModePSResources;
