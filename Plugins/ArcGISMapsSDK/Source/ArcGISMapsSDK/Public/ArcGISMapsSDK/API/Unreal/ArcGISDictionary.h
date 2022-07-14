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

namespace Esri
{
namespace GameEngine
{
namespace Security
{
class ArcGISAuthenticationConfiguration;
} // namespace Security
} // namespace GameEngine

namespace Unreal
{
ARCGISMAPSSDK_API void DestroyDictionary(void* object);

template <typename Key, typename Value>
class ArcGISDictionary final
{
public:
	ArcGISDictionary();

	~ArcGISDictionary()
	{
		DestroyDictionary(this);
	}

	explicit ArcGISDictionary(void* handle) : m_handle(handle)
	{
	}

	ArcGISDictionary(ArcGISDictionary&& other)
	{
		using std::swap;
		swap(m_handle, other.m_handle);
	}

	ArcGISDictionary(const ArcGISDictionary&) = delete;

	ArcGISDictionary& operator=(const ArcGISDictionary&) = delete;

	ArcGISDictionary& operator=(ArcGISDictionary&& other) noexcept
	{
		if (this != &other)
		{
			using std::swap;
			swap(m_handle, other.m_handle);
		}

		return *this;
	}

	Value At(const Key& key) const;

	bool Contains(const Key& key) const;

	size_t GetSize() const;

	void Insert(const Key& key, const Value& value);

	void Remove(const Key& key);

	void RemoveAll();

	void Replace(const Key& key, const Value& newValue);

	void* GetHandle() const
	{
		return m_handle;
	}

private:
	void* m_handle{nullptr};
};

template <>
ARCGISMAPSSDK_API ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::ArcGISDictionary();

template <>
ARCGISMAPSSDK_API ArcGISDictionary<FString, FString>::ArcGISDictionary();

template <>
ARCGISMAPSSDK_API GameEngine::Security::ArcGISAuthenticationConfiguration
ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::At(const FString& key) const;

template <>
ARCGISMAPSSDK_API FString ArcGISDictionary<FString, FString>::At(const FString& key) const;

template <>
ARCGISMAPSSDK_API bool ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::Contains(const FString& key) const;

template <>
ARCGISMAPSSDK_API bool ArcGISDictionary<FString, FString>::Contains(const FString& key) const;

template <>
ARCGISMAPSSDK_API size_t ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::GetSize() const;

template <>
ARCGISMAPSSDK_API size_t ArcGISDictionary<FString, FString>::GetSize() const;

template <>
ARCGISMAPSSDK_API void ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::Insert(
	const FString& key,
	const GameEngine::Security::ArcGISAuthenticationConfiguration& value);

template <>
ARCGISMAPSSDK_API void ArcGISDictionary<FString, FString>::Insert(const FString& key, const FString& value);

template <>
ARCGISMAPSSDK_API void ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::Remove(const FString& key);

template <>
ARCGISMAPSSDK_API void ArcGISDictionary<FString, FString>::Remove(const FString& key);

template <>
ARCGISMAPSSDK_API void ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::RemoveAll();

template <>
ARCGISMAPSSDK_API void ArcGISDictionary<FString, FString>::RemoveAll();

template <>
ARCGISMAPSSDK_API void ArcGISDictionary<FString, GameEngine::Security::ArcGISAuthenticationConfiguration>::Replace(
	const FString& key,
	const GameEngine::Security::ArcGISAuthenticationConfiguration& newValue);

template <>
ARCGISMAPSSDK_API void ArcGISDictionary<FString, FString>::Replace(const FString& key, const FString& newValue);
} // namespace Unreal
} // namespace Esri
