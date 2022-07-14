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

#include "ArcGISMapsSDK/API/Unreal/ArcGISList.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/Standard/ArcGISIntermediateMutableArray.h"

namespace Esri
{
namespace Unreal
{
// This is a walkaround for macOS where the destructor symbol is not being exported as expected
void DestroyList(void* object)
{
	auto intermediateMutableArray = reinterpret_cast<Standard::ArcGISIntermediateMutableArray*>(object);

	intermediateMutableArray->~ArcGISIntermediateMutableArray();
}
} // namespace Unreal
} // namespace Esri
