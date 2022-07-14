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

#include "ArcGISMapsSDK/Utils/Unreal/AtmosphereHelpers.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISSpheroidData.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoUtils.h"

#if WITH_EDITOR
#include "EditorViewportClient.h"
#endif

FVector3d UAtmosphereHelpers::GetGlobalViewSkyAtmospherePositionFrom(UArcGISMapComponent* InMapComponent, const FVector3d& Position)
{
	using namespace Esri::ArcGISMapsSDK::Utils::GeoCoord;

	if (InMapComponent->HasSpatialReference())
	{
		double ToCentimeters = 100.0;

		auto UnrealEllipsoidCenter = InMapComponent->ToEnginePosition(FVector3d(0, 0, 0));

		auto GeographicCoordinates = InMapComponent->EngineToGeographic(Position);

		if (GeographicCoordinates.IsValid())
		{
			auto GeoP = FGeoPosition{GeographicCoordinates.X, GeographicCoordinates.Y, 0.0, GeographicCoordinates.GetWKID()};
			auto xyz = InMapComponent->GeographicToEngine(GeoP);

			auto EllipsoidRadius = (xyz - UnrealEllipsoidCenter).Length();

			FVector3d dir = Position - UnrealEllipsoidCenter;
			dir.Normalize(0);

			double GlobeRadius = Esri::GameEngine::Geometry::ArcGISSpatialReference::WGS84().GetSpheroidData().MajorSemiAxis;

			return UnrealEllipsoidCenter - (ToCentimeters * GlobeRadius - EllipsoidRadius) * dir;
		}
	}

	return FVector3d();
}

FVector3d UAtmosphereHelpers::GetLocalViewSkyAtmospherePositionFrom(const FVector3d& Position)
{
	using namespace Esri::ArcGISMapsSDK::Utils::GeoCoord;

	double GlobeRadius = Esri::GameEngine::Geometry::ArcGISSpatialReference::WGS84().GetSpheroidData().MajorSemiAxis;
	double ToCentimeters = 100.0;
	return FVector3d(Position.X, Position.Y, -ToCentimeters * GlobeRadius);
}

float UAtmosphereHelpers::GetAerialPerspectiveViewDistanceScaleFrom(UArcGISMapComponent* InMapComponent,
																	const FVector3d& Position,
																	float AtmosphereHeight,
																	float AtmosphereScale,
																	float BlendingWidth)
{
	if (InMapComponent->HasSpatialReference())
	{
		auto GeographicCoordinates = InMapComponent->EngineToGeographic(Position);

		if (GeographicCoordinates.IsValid())
		{
			float Altitude = static_cast<float>(GeographicCoordinates.Z);

			return std::min(1.0f, (Altitude - 1000.0f * AtmosphereHeight) / BlendingWidth) * AtmosphereScale;
		}
	}

	return 0.0f;
}
