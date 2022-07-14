// COPYRIGHT 1995-2022 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Attn: Contracts and Legal Department
// Environmental Systems Research Institute, Inc.
// 380 New York Street
// Redlands, California 92373
// USA
//
// email: legal@esri.com
#pragma once

#include "VectorTypes.h"

#include "ArcGISMapsSDK/Utils/GeoCoord/GeoPosition.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Utils
{
namespace GeoCoord
{
class GeoUtils
{
public:
	static FVector3d GetGeodeticSurfaceNormal(const FVector3d& InCartesianPosition,
											  const Esri::GameEngine::Geometry::ArcGISSpheroidData& SpheroidData);
	static FMatrix44d GetENUReferenceMatrix(const FVector3d& InCartesianPosition, const Esri::GameEngine::Geometry::ArcGISSpheroidData& SpheroidData);
	static FGeoPosition WebMercatorToWGS84LonLatAlt(const FVector3d& InCartesianPosition);
	static FGeoPosition WebMercatorTileToWGS84LonLatAlt(double InX, double InY, uint32_t lod);
};
} // namespace GeoCoord
} // namespace Utils
} // namespace ArcGISMapsSDK
} // namespace Esri
