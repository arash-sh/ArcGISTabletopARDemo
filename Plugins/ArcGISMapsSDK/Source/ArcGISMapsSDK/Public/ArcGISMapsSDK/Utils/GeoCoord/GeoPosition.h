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

#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISPoint.h"
#include "ArcGISMapsSDK/API/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISPoint.h"

#include "GeoPosition.generated.h"

USTRUCT(BlueprintType)
struct ARCGISMAPSSDK_API FGeoPosition
{
	GENERATED_BODY()

public:
	FGeoPosition();
	FGeoPosition(double InX, double InY, double InZ);
	FGeoPosition(double InX, double InY, double InZ, int InSpatialReferenceWkid);
	FGeoPosition(double InX, double InY, double InZ, const Esri::GameEngine::Geometry::ArcGISSpatialReference& InSpatialReference);
	FGeoPosition(const Esri::GameEngine::Geometry::ArcGISPoint& InPosition);
	FGeoPosition(UArcGISPoint* InPosition);

	operator Esri::GameEngine::Geometry::ArcGISPoint() const;
	operator UArcGISPoint*() const;
	bool operator==(const FGeoPosition& other) const;
	bool operator!=(const FGeoPosition& other) const;

	bool IsValid() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ArcGISMapsSDK|Coordinates")
	double X = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ArcGISMapsSDK|Coordinates")
	double Y = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ArcGISMapsSDK|Coordinates")
	double Z = 0;

	// An exception can be thrown when directly accessing this if the WKID is a bad value
	// clang-format off
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "ArcGISMapsSDK|Coordinates", meta = (DisplayName = "Spatial Reference WKID", ToolTip = "The spatial reference's well known ID defines the spatial reference of the point.\nSee our documentation on spatial references to learn more.\nUsing a spatial reference that differs from the one used by the ArcGISMap can result in position inaccuracies from reprojection."))
	mutable int SpatialReferenceWkid = 4326; // WGS84 by default
	// clang-format on

	int GetWKID() const;
};
