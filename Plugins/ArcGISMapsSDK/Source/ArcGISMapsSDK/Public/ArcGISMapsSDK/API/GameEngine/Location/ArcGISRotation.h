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

#pragma once

namespace Esri
{
namespace GameEngine
{
namespace Location
{
class ARCGISMAPSSDK_API ArcGISRotation
{
public:
#pragma region Constructors
	/// Creates a new ArcGISRotation.
	///
	/// - Remark: Creates a new ArcGISRotation.
	ArcGISRotation(double pitch, double roll, double heading);
#pragma endregion Constructors

#pragma region Properties
	/// Roll rotation axis
	///
	double GetHeading() const;

	/// Pitch rotation axis
	///
	double GetPitch() const;

	/// Roll rotation axis
	///
	double GetRoll() const;
#pragma endregion Properties

private:
	double Heading;
	double Pitch;
	double Roll;
};
} // namespace Location
} // namespace GameEngine
} // namespace Esri
