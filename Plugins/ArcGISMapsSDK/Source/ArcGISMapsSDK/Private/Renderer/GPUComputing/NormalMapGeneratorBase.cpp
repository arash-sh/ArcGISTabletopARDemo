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
#include "ArcGISMapsSDK/Renderer/GPUComputing/NormalMapGeneratorBase.h"

#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISAngularUnit.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISGeodeticCurveType.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISGeodeticDistanceResult.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISGeometryEngine.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISLinearUnit.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISPoint.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISSpheroidData.h"
#include "ArcGISMapsSDK/API/GameEngine/View/ArcGISView.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace GPUComputing
{
FNormalMapGeneratorBase::FNormalMapGeneratorBase(TSharedPtr<Esri::GameEngine::View::ArcGISView> InView) : View(std::move(InView))
{
}

FNormalMapGeneratorBase::~FNormalMapGeneratorBase() = default;

void FNormalMapGeneratorBase::Compute(const TSharedPtr<GPUResources::FGPUResourceTexture2D>& InputElevation,
									  const FVector4f& TileExtent,
									  const FVector4f& TextureExtent,
									  const TSharedPtr<const GPUResources::FGPUResourceRenderTarget>& Output)
{
	using namespace Esri::GameEngine::Geometry;

	const auto& sr = View->GetSpatialReference();

	if (!sr)
	{
		return;
	}
	else if (sr.GetIsGeographic())
	{
		double GlobeRadius = sr.GetSpheroidData().MajorSemiAxis;
		float circleLongitude = 2.0 * PI * GlobeRadius;
		float minLatitude = FMath::DegreesToRadians(TileExtent.Y);
		float latitudeAngleDelta = FMath::DegreesToRadians(TileExtent.W / Output->GetHeight());
		float longitudeArc = FMath::Abs(TileExtent.Z) / 360.0;

		ArcGISGeodeticDistanceResult result = ArcGISGeometryEngine::DistanceGeodetic(
			ArcGISPoint(TileExtent.X, TileExtent.Y, sr), ArcGISPoint(TileExtent.X, TileExtent.Y + TileExtent.W, sr), ArcGISLinearUnit{},
			ArcGISAngularUnit{}, ArcGISGeodeticCurveType::Geodesic);

		// Assumption - this is always positive, since it's named distance
		float latitudeLength = result.GetDistance();

		DrawGlobalViewMode(InputElevation, TextureExtent, circleLongitude, minLatitude, latitudeAngleDelta, longitudeArc, latitudeLength, Output);
	}
	else
	{
		DrawLocalViewMode(InputElevation, TextureExtent, TileExtent.W, TileExtent.Z, Output);
	}
}

} // namespace GPUComputing
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
