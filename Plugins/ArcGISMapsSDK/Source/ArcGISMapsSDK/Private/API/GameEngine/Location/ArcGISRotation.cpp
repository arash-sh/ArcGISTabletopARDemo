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

#include "ArcGISMapsSDK/API/GameEngine/Location/ArcGISRotation.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/Standard/ArcGISErrorHandler.h"

namespace Esri
{
namespace GameEngine
{
namespace Location
{
ArcGISRotation::ArcGISRotation(double pitch, double roll, double heading) : Heading(heading), Pitch(pitch), Roll(roll)
{
}

double ArcGISRotation::GetHeading() const
{
	return Heading;
}

double ArcGISRotation::GetPitch() const
{
	return Pitch;
}

double ArcGISRotation::GetRoll() const
{
	return Roll;
}
} // namespace Location
} // namespace GameEngine
} // namespace Esri
