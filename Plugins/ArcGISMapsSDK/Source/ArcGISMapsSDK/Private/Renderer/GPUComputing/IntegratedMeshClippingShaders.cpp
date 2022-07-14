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
#include "ArcGISMapsSDK/Renderer/GPUComputing/IntegratedMeshClippingShaders.h"

#include "RHIStaticStates.h"
#include "ShaderParameterUtils.h"

// Integrated mesh clipping vertex shader

IMPLEMENT_SHADER_TYPE(,
	FIntegratedMeshClippingVS,
	TEXT("/Plugin/ArcGISMapsSDK/Private/IntegratedMeshClipping.usf"),
	TEXT("IntegratedMeshClippingVS"),
	SF_Vertex);


// Integrated mesh clipping pixel shader

IMPLEMENT_SHADER_TYPE(,
	FIntegratedMeshClippingOBBDepthPassPS,
	TEXT("/Plugin/ArcGISMapsSDK/Private/IntegratedMeshClipping.usf"),
	TEXT("IntegratedMeshClippingOBBDepthPassPS"),
	SF_Pixel);

IMPLEMENT_SHADER_TYPE(,
	FIntegratedMeshClippingStencilPassPS,
	TEXT("/Plugin/ArcGISMapsSDK/Private/IntegratedMeshClipping.usf"),
	TEXT("IntegratedMeshClippingStencilPassPS"),
	SF_Pixel);

IMPLEMENT_SHADER_TYPE(,
	FIntegratedMeshClippingClearToZeroPassPS,
	TEXT("/Plugin/ArcGISMapsSDK/Private/IntegratedMeshClipping.usf"),
	TEXT("IntegratedMeshClippingClearToZeroPassPS"),
	SF_Pixel);

TGlobalResource<FIntegratedMeshClippingPSResources> GIntegratedMeshClippingPSResources;
