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

#include "ArcGISMapsSDK/API/Unreal/ArcGISArrayBuilder.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISVisualizationAttributeDescription.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISElement.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISIntermediateImmutableArrayBuilder.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISImmutableArray.h"

namespace Esri
{
namespace Unreal
{
template <>
void ArcGISArrayBuilder<FString>::Add(const FString& value)
{
	auto intermediateImmutableArrayBuilder = reinterpret_cast<Standard::ArcGISIntermediateImmutableArrayBuilder*>(this);

	intermediateImmutableArrayBuilder->Add(Standard::ArcGISElement::FromString(value));
}

template <>
void ArcGISArrayBuilder<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::Add(
	const GameEngine::Attributes::ArcGISVisualizationAttributeDescription& value)
{
	auto intermediateImmutableArrayBuilder = reinterpret_cast<Standard::ArcGISIntermediateImmutableArrayBuilder*>(this);

	intermediateImmutableArrayBuilder->Add(Standard::ArcGISElement::FromArcGISVisualizationAttributeDescription(value));
}

template <>
Unreal::ArcGISImmutableArray<FString> ArcGISArrayBuilder<FString>::MoveToArray()
{
	auto intermediateImmutableArrayBuilder = reinterpret_cast<Standard::ArcGISIntermediateImmutableArrayBuilder*>(this);

	auto immutableList = intermediateImmutableArrayBuilder->MoveToArray();

	auto handle = immutableList.GetHandle();

	immutableList.SetHandle(nullptr);

	return Unreal::ArcGISImmutableArray<FString>(handle);
}

template <>
Unreal::ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>
ArcGISArrayBuilder<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::MoveToArray()
{
	auto intermediateImmutableArrayBuilder = reinterpret_cast<Standard::ArcGISIntermediateImmutableArrayBuilder*>(this);

	auto immutableList = intermediateImmutableArrayBuilder->MoveToArray();

	auto handle = immutableList.GetHandle();

	immutableList.SetHandle(nullptr);

	return Unreal::ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>(handle);
}
} // namespace Unreal
} // namespace Esri
