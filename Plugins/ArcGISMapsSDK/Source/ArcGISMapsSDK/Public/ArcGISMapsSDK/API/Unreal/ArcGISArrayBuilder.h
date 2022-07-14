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
class ArcGISVisualizationAttributeDescription;
} // namespace Attributes
} // namespace GameEngine

namespace Unreal
{
template <typename T>
class ArcGISImmutableArray;

template <typename T>
class ArcGISArrayBuilder
{
public:
#pragma region Methods
	/// Add a value to the ArcGISArrayBuilder.
	///
	/// - Parameter value: The value that is to be added.
	/// - Since: 100.9.0
	void Add(const T& value);

	/// Creates a Array containing the values added to this ArcGISArrayBuilder.
	///
	/// - Remark: The order of the values in the returned Array matches the order in which the
	/// values were added to this ArcGISArrayBuilder.
	///
	/// This call empties this ArcGISArrayBuilder of values, but leaves it in a valid
	/// (re-usable) state.
	/// - Returns: A Array containing the values added to this ArcGISArrayBuilder.
	/// - Since: 100.9.0
	Unreal::ArcGISImmutableArray<T> MoveToArray();
#pragma endregion Methods

public:
#pragma region Special Members
	explicit ArcGISArrayBuilder(void* handle) : m_handle(handle)
	{
	}

	ArcGISArrayBuilder(ArcGISArrayBuilder<T>&& other) noexcept
	{
		using std::swap;
		swap(m_handle, other.m_handle);
	}

	ArcGISArrayBuilder(const ArcGISArrayBuilder<T>&) = delete;

	ArcGISArrayBuilder<T>& operator=(const ArcGISArrayBuilder<T>&) = delete;

	ArcGISArrayBuilder<T>& operator=(ArcGISArrayBuilder<T>&& other) noexcept
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
ARCGISMAPSSDK_API void ArcGISArrayBuilder<FString>::Add(const FString& value);

template <>
ARCGISMAPSSDK_API void ArcGISArrayBuilder<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::Add(
	const GameEngine::Attributes::ArcGISVisualizationAttributeDescription& value);

template <>
ARCGISMAPSSDK_API Unreal::ArcGISImmutableArray<FString> ArcGISArrayBuilder<FString>::MoveToArray();

template <>
ARCGISMAPSSDK_API Unreal::ArcGISImmutableArray<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>
ArcGISArrayBuilder<GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::MoveToArray();
} // namespace Unreal
} // namespace Esri
