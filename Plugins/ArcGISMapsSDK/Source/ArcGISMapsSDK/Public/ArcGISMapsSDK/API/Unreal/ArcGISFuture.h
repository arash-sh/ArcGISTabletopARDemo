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
using FutureCompletedEvent = std::function<void()>;

class ArcGISFutureBase
{
public:
	ArcGISFutureBase() = default;
	explicit ArcGISFutureBase(void* handle);
	ArcGISFutureBase(const ArcGISFutureBase&) = delete;
	ArcGISFutureBase(ArcGISFutureBase&& other) noexcept;
	~ArcGISFutureBase();

	ArcGISFutureBase& operator=(const ArcGISFutureBase&) = delete;
	ArcGISFutureBase& operator=(ArcGISFutureBase&& other) noexcept;

	void* GetHandle() const;

	template <typename T>
	T Get() const;

	void SetTaskCompleted(Unreal::FutureCompletedEvent taskCompleted);

private:
	void* m_handle{nullptr};
	Unreal::FutureCompletedEvent m_taskCompleted;
};

template <typename T>
class ArcGISFuture
{
public:
	ArcGISFuture() = default;

	explicit ArcGISFuture(void* handle) : futureBase(ArcGISFutureBase(handle))
	{
	}

	ArcGISFuture(const ArcGISFuture<T>&) = delete;

	ArcGISFuture(ArcGISFuture<T>&& other) noexcept
	{
		using std::swap;

		swap(futureBase, other.futureBase);
	}

	ArcGISFuture<T>& operator=(const ArcGISFuture<T>&) = delete;

	ArcGISFuture<T>& operator=(ArcGISFuture<T>&& other) noexcept
	{
		if (this != &other)
		{
			using std::swap;

			swap(futureBase, other.futureBase);
		}

		return *this;
	}

	void* GetHandle() const
	{
		return futureBase.GetHandle();
	}

	T Get() const
	{
		return futureBase.Get<T>();
	}

	void SetTaskCompleted(Unreal::FutureCompletedEvent taskCompleted)
	{
		futureBase.SetTaskCompleted(taskCompleted);
	}

private:
	ArcGISFutureBase futureBase;
};

template <>
ARCGISMAPSSDK_API uint32_t ArcGISFutureBase::Get() const;
} // namespace Unreal
} // namespace Esri
