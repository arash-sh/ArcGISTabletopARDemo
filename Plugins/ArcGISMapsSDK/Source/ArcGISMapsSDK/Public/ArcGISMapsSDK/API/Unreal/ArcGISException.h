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
class ArcGISException : public std::exception
{
public:
	ArcGISException() = default;
	explicit ArcGISException(void* handle);
	ArcGISException(const ArcGISException&) = delete;
	ArcGISException(ArcGISException&& other) noexcept;
	ARCGISMAPSSDK_API ~ArcGISException() override;

	ArcGISException& operator=(const ArcGISException&) = delete;
	ArcGISException& operator=(ArcGISException&& other) noexcept;

	explicit operator bool() const noexcept;

	ARCGISMAPSSDK_API FString GetAdditionalMessage() const;
	ARCGISMAPSSDK_API int32_t GetCode() const;
	ARCGISMAPSSDK_API FString GetMessage() const;

	ARCGISMAPSSDK_API const char* what() const noexcept override;

private:
	void* m_handle{nullptr};

	std::string AdditionalMessage;
	std::string Message;
};
} // namespace Unreal
} // namespace Esri
