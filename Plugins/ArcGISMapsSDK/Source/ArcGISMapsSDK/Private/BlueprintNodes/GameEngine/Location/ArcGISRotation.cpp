// COPYRIGHT 1995-2021 ESRI
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

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Location/ArcGISRotation.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/GameEngine/Location/ArcGISRotation.h"

UArcGISRotation* UArcGISRotation::CreateArcGISRotation(double pitch, double roll, double heading)
{
	auto result = NewObject<UArcGISRotation>();

	result->APIObject = ::MakeShared<Esri::GameEngine::Location::ArcGISRotation>(pitch, roll, heading);

	return result;
}

double UArcGISRotation::GetHeading()
{
	return APIObject->GetHeading();
}

double UArcGISRotation::GetPitch()
{
	return APIObject->GetPitch();
}

double UArcGISRotation::GetRoll()
{
	return APIObject->GetRoll();
}
