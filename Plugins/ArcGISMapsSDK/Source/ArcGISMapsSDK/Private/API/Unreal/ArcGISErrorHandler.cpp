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

#include "ArcGISMapsSDK/API/Unreal/ArcGISErrorHandler.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/Unreal/ArcGISException.h"
#include "ArcGISMapsSDK/CAPI/Standard/Error.h"

namespace Esri
{
namespace Unreal
{
RT_ErrorHandler CreateErrorHandler()
{
	RT_ErrorHandler errorHandler;

	static auto callbackHandler = [](void* userData, RT_ErrorHandle error) {
		if (error)
		{
			throw Unreal::ArcGISException{error};
		}
	};

	errorHandler.handler = callbackHandler;

	return errorHandler;
}
} // namespace Unreal
} // namespace Esri
