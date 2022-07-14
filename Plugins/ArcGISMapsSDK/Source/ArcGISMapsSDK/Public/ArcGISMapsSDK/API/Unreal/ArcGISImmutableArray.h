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
namespace Attributes
{
class ArcGISAttribute;
class ArcGISVisualizationAttribute;
class ArcGISVisualizationAttributeDescription;
} // namespace Attributes
} // namespace GameEngine

namespace Unreal
{
template <typename T>
class ArcGISArrayBuilder;

template <typename T>
class ArcGISImmutableArray
{
public:
#pragma region Methods
	/// Get a value at a specific position.
	///
	/// - Remark: Retrieves the value at the specified position.
	/// - Parameter position: The position which you want to get the value.
	/// - Returns: The value, T, at the position requested.
	/// - Since: 100.0.0
	T At(size_t position) const;

	/// Creates a ArcGISArrayBuilder.
	///
	/// - Returns: A ArcGISArrayBuilder
	/// - SeeAlso: ArcGISArrayBuilder
	/// - Since: 100.9.0
	static ArcGISArrayBuilder<T> CreateBuilder();
#pragma endregion Methods

public:
#pragma region Special Members
	ArcGISImmutableArray() = default;

	explicit ArcGISImmutableArray(void* handle) : m_handle(handle)
	{
	}

	ArcGISImmutableArray(ArcGISImmutableArray<T>&& other) noexcept
	{
		using std::swap;
		swap(m_handle, other.m_handle);
	}

	ArcGISImmutableArray(const ArcGISImmutableArray<T>&) = delete;

	ArcGISImmutableArray<T>& operator=(const ArcGISImmutableArray<T>&) = delete;

	ArcGISImmutableArray<T>& operator=(ArcGISImmutableArray<T>&& other) noexcept
	{
		if (this != &other)
		{
			using std::swap;
			swap(m_handle, other.m_handle);
		}

		return *this;
	}

	void* GetHandle() const
	{
		return m_handle;
	}

	void SetHandle(void* handle)
	{
		m_handle = handle;
	}
#pragma endregion Special Members

protected:
	void* m_handle{nullptr};
};

template <>
ARCGISMAPSSDK_API FString ArcGISImmutableArray<FString>::At(size_t position) const;

template <>
ARCGISMAPSSDK_API GameEngine::Attributes::ArcGISAttribute ArcGISImmutableArray<GameEngine::Attributes::ArcGISAttribute>::At(size_t position) const;

template <>
ARCGISMAPSSDK_API GameEngine::Attributes::ArcGISVisualizationAttribute ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttribute>::At(
	size_t position) const;

template <>
ARCGISMAPSSDK_API GameEngine::Attributes::ArcGISVisualizationAttributeDescription
ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::At(size_t position) const;

template <>
ARCGISMAPSSDK_API ArcGISArrayBuilder<FString> ArcGISImmutableArray<FString>::CreateBuilder();

template <>
ARCGISMAPSSDK_API ArcGISArrayBuilder<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>
ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::CreateBuilder();
} // namespace Unreal
} // namespace Esri
