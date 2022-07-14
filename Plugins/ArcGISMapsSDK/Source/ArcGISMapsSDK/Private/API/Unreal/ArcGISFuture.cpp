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

#include "ArcGISMapsSDK/API/Unreal/ArcGISFuture.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/Standard/ArcGISElement.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISIntermediateFuture.h"

namespace Esri
{
namespace Unreal
{
ArcGISFutureBase::ArcGISFutureBase(void* handle) : m_handle{new Standard::ArcGISIntermediateFuture(handle)}
{
}

ArcGISFutureBase::ArcGISFutureBase(ArcGISFutureBase&& other) noexcept : m_handle{other.m_handle}
{
	other.m_handle = nullptr;
}

ArcGISFutureBase::~ArcGISFutureBase()
{
	delete static_cast<Standard::ArcGISIntermediateFuture*>(m_handle);
}

ArcGISFutureBase& ArcGISFutureBase::operator=(ArcGISFutureBase&& other) noexcept
{
	if (this != &other)
	{
		using std::swap;

		swap(m_handle, other.m_handle);
	}

	return *this;
}

void* ArcGISFutureBase::GetHandle() const
{
	return m_handle;
}

template <>
uint32_t ArcGISFutureBase::Get() const
{
	auto intermediateFuture = (Standard::ArcGISIntermediateFuture*)m_handle;

	auto element = intermediateFuture->Get();

	return element.GetValueAsUInt32();
}

void ArcGISFutureBase::SetTaskCompleted(Unreal::FutureCompletedEvent taskCompleted)
{
	m_taskCompleted = std::move(taskCompleted);

	auto intermediateFuture = (Standard::ArcGISIntermediateFuture*)m_handle;

	intermediateFuture->SetTaskCompleted([this]() {
		if (!m_taskCompleted)
		{
			return;
		}

		m_taskCompleted();
	});
}
} // namespace Unreal
} // namespace Esri
