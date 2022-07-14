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
namespace Unreal
{
ARCGISMAPSSDK_API void DestroyList(void* object);

template <typename T>
class ArcGISList final
{
public:
	ArcGISList();

	~ArcGISList()
	{
		DestroyList(this);
	}

	explicit ArcGISList(void* handle) : m_handle(handle)
	{
	}

	ArcGISList(ArcGISList&& other)
	{
		using std::swap;
		swap(m_handle, other.m_handle);
	}

	ArcGISList(const ArcGISList&) = delete;

	ArcGISList& operator=(const ArcGISList&) = delete;

	ArcGISList& operator=(ArcGISList&& other) noexcept
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

private:
	void* m_handle{nullptr};
};
} // namespace Unreal
} // namespace Esri
