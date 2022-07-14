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
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoUtils.h"

#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISGeometryEngine.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISPoint.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISSpheroidData.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Utils
{
namespace GeoCoord
{
FVector3d GeoUtils::GetGeodeticSurfaceNormal(const FVector3d& InCartesianPosition, const Esri::GameEngine::Geometry::ArcGISSpheroidData& SpheroidData)
{
	FVector3d Normal(InCartesianPosition.X, InCartesianPosition.Y, InCartesianPosition.Z / (1.0 - SpheroidData.SquaredEccentricity));

	Normal.Normalize(0);

	return Normal;
}

FMatrix44d GeoUtils::GetENUReferenceMatrix(const FVector3d& InCartesianPosition, const Esri::GameEngine::Geometry::ArcGISSpheroidData& SpheroidData)
{
	FVector3d East;
	FVector3d North;
	FVector3d Up;

	if (FMath::Abs(InCartesianPosition.X) < FMathd::Epsilon && FMath::Abs(InCartesianPosition.Y) < FMathd::Epsilon)
	{
		// At South or North pole - Axis are set to be continuous with other points
		int Sign = FMathd::SignNonZero(InCartesianPosition.Z);

		// At the center, it should never happen
		if (FMath::Abs(InCartesianPosition.Z) < FMathd::Epsilon)
		{
			// Return same as north pole
			Sign = 1;
		}

		Up = FVector3d(0.0, 0.0, 1.0 * Sign);
		East = FVector3d(0.0, 1.0, 0.0);
		North = FVector3d(-1.0 * Sign, 0.0, 0.0);
	}
	else
	{
		Up = GetGeodeticSurfaceNormal(InCartesianPosition, SpheroidData);
		East = FVector3d(InCartesianPosition.Y, -InCartesianPosition.X, 0.0);
		East.Normalize(0);
		North = Up.Cross(East);
	}

	// X in ECEF is Right -> East
	// Y in ECEF is Forward -> North
	// Z in ECEF is Up -> Up
	// clang-format off
	auto ENUReferenceMatrix = FMatrix44d(FPlane4d(East, 0.0),
										 FPlane4d(North, 0.0),
										 FPlane4d(Up, 0.0),
										 FPlane4d(InCartesianPosition, 1.0));
	// clang-format on

	return ENUReferenceMatrix;
}

FGeoPosition GeoUtils::WebMercatorToWGS84LonLatAlt(const FVector3d& InCartesianPosition)
{
	using namespace Esri::GameEngine::Geometry;

	ArcGISPoint point(InCartesianPosition.X, InCartesianPosition.Y, InCartesianPosition.Z, ArcGISSpatialReference::WebMercator());

	auto geometry = ArcGISGeometryEngine::Project(point, ArcGISSpatialReference::WGS84());

	ArcGISPoint geographicPoint(geometry.GetHandle());

	geometry.SetHandle(nullptr);

	return FGeoPosition(geographicPoint.GetX(), geographicPoint.GetY(), geographicPoint.GetZ(), ArcGISSpatialReference::WGS84().GetWKID());
}

FGeoPosition GeoUtils::WebMercatorTileToWGS84LonLatAlt(double InX, double InY, uint32_t lod)
{
	double GlobeRadius = Esri::GameEngine::Geometry::ArcGISSpatialReference::WGS84().GetSpheroidData().MajorSemiAxis;
	auto n = 1 << lod;
	FVector3d wm{PI * (-1 + 2 * InX / n) * GlobeRadius, PI * (+1 - 2 * InY / n) * GlobeRadius, 0};
	return WebMercatorToWGS84LonLatAlt(wm);
}
} // namespace GeoCoord
} // namespace Utils
} // namespace ArcGISMapsSDK
} // namespace Esri
