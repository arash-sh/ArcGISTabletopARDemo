// COPYRIGHT 1995-2022 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Environmental Systems Research Institute, Inc.
// Attn: Contracts and Legal Services Department
// 380 New York Street
// Redlands, California, 92373
// USA
//
// email: contracts@esri.com

#include "ArcGISMapsSDK/API/Unreal/ArcGISImmutableArray.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISAttribute.h"
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISVisualizationAttribute.h"
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISVisualizationAttributeDescription.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISElement.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISElementType.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISIntermediateImmutableArray.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISIntermediateImmutableArrayBuilder.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISArrayBuilder.h"

namespace Esri
{
namespace Unreal
{
template <>
ARCGISMAPSSDK_API FString ArcGISImmutableArray<FString>::At(size_t position) const
{
	auto intermediateImmutableArray = reinterpret_cast<const Standard::ArcGISIntermediateImmutableArray*>(this);

	auto element = intermediateImmutableArray->At(position);

	return element.GetValueAsString();
}

template <>
ARCGISMAPSSDK_API GameEngine::Attributes::ArcGISAttribute ArcGISImmutableArray<GameEngine::Attributes::ArcGISAttribute>::At(size_t position) const
{
	auto intermediateImmutableArray = reinterpret_cast<const Standard::ArcGISIntermediateImmutableArray*>(this);

	auto element = intermediateImmutableArray->At(position);

	return element.GetValueAsArcGISAttribute();
}

template <>
ARCGISMAPSSDK_API GameEngine::Attributes::ArcGISVisualizationAttribute ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttribute>::At(
	size_t position) const
{
	auto intermediateImmutableArray = reinterpret_cast<const Standard::ArcGISIntermediateImmutableArray*>(this);

	auto element = intermediateImmutableArray->At(position);

	return element.GetValueAsArcGISVisualizationAttribute();
}

template <>
ARCGISMAPSSDK_API GameEngine::Attributes::ArcGISVisualizationAttributeDescription
ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::At(size_t position) const
{
	auto intermediateImmutableArray = reinterpret_cast<const Standard::ArcGISIntermediateImmutableArray*>(this);

	auto element = intermediateImmutableArray->At(position);

	return element.GetValueAsArcGISVisualizationAttributeDescription();
}

template <>
ArcGISArrayBuilder<FString> ArcGISImmutableArray<FString>::CreateBuilder()
{
	auto intermediateArrayBuilder = Standard::ArcGISIntermediateImmutableArray::CreateBuilder(Standard::ArcGISElementType::String);

	auto handle = intermediateArrayBuilder.GetHandle();

	intermediateArrayBuilder.SetHandle(nullptr);

	return ArcGISArrayBuilder<FString>(handle);
}

template <>
ArcGISArrayBuilder<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>
ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::CreateBuilder()
{
	auto intermediateArrayBuilder =
		Standard::ArcGISIntermediateImmutableArray::CreateBuilder(Standard::ArcGISElementType::VisualizationAttributeDescription);

	auto handle = intermediateArrayBuilder.GetHandle();

	intermediateArrayBuilder.SetHandle(nullptr);

	return ArcGISArrayBuilder<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>(handle);
}
} // namespace Unreal
} // namespace Esri
