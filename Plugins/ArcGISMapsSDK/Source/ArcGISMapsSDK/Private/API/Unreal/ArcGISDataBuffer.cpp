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

#include "ArcGISMapsSDK/API/Unreal/ArcGISDataBuffer.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/GameEngine/ArcGISIntermediateDataBuffer.h"

namespace Esri
{
namespace Unreal
{
void* ArcGISDataBufferBase::GetData() const
{
	auto intermediateDataBuffer = reinterpret_cast<const GameEngine::ArcGISIntermediateDataBuffer*>(this);

	return intermediateDataBuffer->GetData();
}

size_t ArcGISDataBufferBase::GetItemCount() const
{
	auto intermediateDataBuffer = reinterpret_cast<const GameEngine::ArcGISIntermediateDataBuffer*>(this);

	return intermediateDataBuffer->GetSizeBytes() / intermediateDataBuffer->GetItemSize();
}

size_t ArcGISDataBufferBase::GetItemSize() const
{
	auto intermediateDataBuffer = reinterpret_cast<const GameEngine::ArcGISIntermediateDataBuffer*>(this);

	return intermediateDataBuffer->GetItemSize();
}

size_t ArcGISDataBufferBase::GetSizeBytes() const
{
	auto intermediateDataBuffer = reinterpret_cast<const GameEngine::ArcGISIntermediateDataBuffer*>(this);

	return intermediateDataBuffer->GetSizeBytes();
}

ArcGISDataBufferBase::ArcGISDataBufferBase(void* handle) : m_handle{handle}
{
}

ArcGISDataBufferBase::ArcGISDataBufferBase(ArcGISDataBufferBase&& other) noexcept : m_handle{other.m_handle}
{
	other.m_handle = nullptr;
}

ArcGISDataBufferBase::~ArcGISDataBufferBase()
{
	auto intermediateDataBuffer = reinterpret_cast<GameEngine::ArcGISIntermediateDataBuffer*>(this);

	intermediateDataBuffer->~ArcGISIntermediateDataBuffer();
}

ArcGISDataBufferBase& ArcGISDataBufferBase::operator=(ArcGISDataBufferBase&& other) noexcept
{
	if (this != &other)
	{
		using std::swap;

		swap(m_handle, other.m_handle);
	}

	return *this;
}

ArcGISDataBufferBase::operator bool() const noexcept
{
	return m_handle != nullptr;
}
} // namespace Unreal
} // namespace Esri
