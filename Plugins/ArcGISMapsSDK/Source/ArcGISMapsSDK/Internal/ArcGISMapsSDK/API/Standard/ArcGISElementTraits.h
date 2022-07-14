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

#pragma once

#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationConfiguration.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISElement.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISElementType.h"

namespace Esri
{
namespace Standard
{
template <typename T>
struct ElementTraits;

template <>
struct ElementTraits<FString>
{
	static constexpr decltype(&ArcGISElement::FromString) from{&ArcGISElement::FromString};
	static FString to(ArcGISElement element)
	{
		return element.GetValueAsString();
	}
	static constexpr ArcGISElementType type{ArcGISElementType::String};
};

template <>
struct ElementTraits<GameEngine::Security::ArcGISAuthenticationConfiguration>
{
	static constexpr decltype(&ArcGISElement::FromArcGISAuthenticationConfiguration) from{&ArcGISElement::FromArcGISAuthenticationConfiguration};
	static GameEngine::Security::ArcGISAuthenticationConfiguration to(ArcGISElement element)
	{
		return element.GetValueAsArcGISAuthenticationConfiguration();
	}
	static constexpr ArcGISElementType type{ArcGISElementType::ArcGISAuthenticationConfiguration};
};
} // namespace Standard
} // namespace Esri
