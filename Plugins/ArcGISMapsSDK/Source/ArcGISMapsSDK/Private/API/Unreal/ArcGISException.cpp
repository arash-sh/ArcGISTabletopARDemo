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

#include "ArcGISMapsSDK/API/Unreal/ArcGISException.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/Standard/ArcGISError.h"

namespace Esri
{
namespace Unreal
{
ArcGISException::ArcGISException(void* handle) : m_handle{handle}
{
	if (m_handle)
	{
		auto error = reinterpret_cast<const Standard::ArcGISError*>(&m_handle);

		AdditionalMessage = TCHAR_TO_ANSI(*error->GetAdditionalMessage());
		Message = TCHAR_TO_ANSI(*error->GetMessage());
	}
}

ArcGISException::ArcGISException(ArcGISException&& other) noexcept = default;

ArcGISException::~ArcGISException() = default;

ArcGISException& ArcGISException::operator=(ArcGISException&& other) noexcept = default;

ArcGISException::operator bool() const noexcept
{
	return m_handle != nullptr;
}

FString ArcGISException::GetAdditionalMessage() const
{
	return AdditionalMessage.c_str();
}

int32_t ArcGISException::GetCode() const
{
	auto error = reinterpret_cast<const Standard::ArcGISError*>(&m_handle);

	return error->GetCode();
}

FString ArcGISException::GetMessage() const
{
	return Message.c_str();
}

const char* ArcGISException::what() const noexcept
{
	return Message.c_str();
}
} // namespace Unreal
} // namespace Esri
