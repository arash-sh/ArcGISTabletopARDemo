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

#include "ArcGISMapsSDK/API/Unreal/ArcGISDictionary.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationConfiguration.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISElementTraits.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISIntermediateDictionary.h"

namespace Esri
{
namespace Unreal
{
// This is a walkaround for macOS where the destructor symbol is not being exported as expected
void DestroyDictionary(void* object)
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(object);

	intermediateDictionary->~ArcGISIntermediateDictionary();
}

template <>
ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::ArcGISDictionary()
{
	new (this) Standard::ArcGISIntermediateDictionary(Standard::ElementTraits<FString>::type,
													  Standard::ElementTraits<GameEngine::Security::ArcGISAuthenticationConfiguration>::type);
}

template <>
ArcGISDictionary<FString, FString>::ArcGISDictionary()
{
	new (this) Standard::ArcGISIntermediateDictionary(Standard::ElementTraits<FString>::type, Standard::ElementTraits<FString>::type);
}

template <>
GameEngine::Security::ArcGISAuthenticationConfiguration ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::At(
	const FString& key) const
{
	auto intermediateDictionary = reinterpret_cast<const Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);
	auto valueElement = intermediateDictionary->At(keyElement);

	return Standard::ElementTraits<GameEngine::Security::ArcGISAuthenticationConfiguration>::to(std::move(valueElement));
}

template <>
FString ArcGISDictionary<FString, FString>::At(const FString& key) const
{
	auto intermediateDictionary = reinterpret_cast<const Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);
	auto valueElement = intermediateDictionary->At(keyElement);

	return Standard::ElementTraits<FString>::to(std::move(valueElement));
}

template <>
bool ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::Contains(const FString& key) const
{
	auto intermediateDictionary = reinterpret_cast<const Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);

	return intermediateDictionary->Contains(keyElement);
}

template <>
bool ArcGISDictionary<FString, FString>::Contains(const FString& key) const
{
	auto intermediateDictionary = reinterpret_cast<const Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);

	return intermediateDictionary->Contains(keyElement);
}

template <>
ARCGISMAPSSDK_API size_t ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::GetSize() const
{
	auto intermediateDictionary = reinterpret_cast<const Standard::ArcGISIntermediateDictionary*>(this);

	return intermediateDictionary->GetSize();
}

template <>
ARCGISMAPSSDK_API size_t ArcGISDictionary<FString, FString>::GetSize() const
{
	auto intermediateDictionary = reinterpret_cast<const Standard::ArcGISIntermediateDictionary*>(this);

	return intermediateDictionary->GetSize();
}

template <>
void ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::Insert(
	const FString& key,
	const GameEngine::Security::ArcGISAuthenticationConfiguration& value)
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);
	auto valueElement = Standard::ElementTraits<GameEngine::Security::ArcGISAuthenticationConfiguration>::from(value);

	intermediateDictionary->Insert(keyElement, valueElement);
}

template <>
void ArcGISDictionary<FString, FString>::Insert(const FString& key, const FString& value)
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);
	auto valueElement = Standard::ElementTraits<FString>::from(value);

	intermediateDictionary->Insert(keyElement, valueElement);
}

template <>
void ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::Remove(const FString& key)
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);

	intermediateDictionary->Remove(keyElement);
}

template <>
void ArcGISDictionary<FString, FString>::Remove(const FString& key)
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);

	intermediateDictionary->Remove(keyElement);
}

template <>
void ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::RemoveAll()
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(this);

	intermediateDictionary->RemoveAll();
}

template <>
void ArcGISDictionary<FString, FString>::RemoveAll()
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(this);

	intermediateDictionary->RemoveAll();
}

template <>
void ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::Replace(
	const FString& key,
	const GameEngine::Security::ArcGISAuthenticationConfiguration& newValue)
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);
	auto valueElement = Standard::ElementTraits<GameEngine::Security::ArcGISAuthenticationConfiguration>::from(newValue);

	intermediateDictionary->Replace(keyElement, valueElement);
}

template <>
void ArcGISDictionary<FString, FString>::Replace(const FString& key, const FString& newValue)
{
	auto intermediateDictionary = reinterpret_cast<Standard::ArcGISIntermediateDictionary*>(this);

	auto keyElement = Standard::ElementTraits<FString>::from(key);
	auto valueElement = Standard::ElementTraits<FString>::from(newValue);

	intermediateDictionary->Replace(keyElement, valueElement);
}
} // namespace Unreal
} // namespace Esri
