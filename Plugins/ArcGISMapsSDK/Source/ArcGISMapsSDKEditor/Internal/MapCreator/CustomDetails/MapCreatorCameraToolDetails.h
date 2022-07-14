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

#include "CoreMinimal.h"
#include "DetailCategoryBuilder.h"
#include "IDetailCustomization.h"
#include "Widgets/Input/SButton.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Location/ArcGISRotation.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoPosition.h"
#include "ArcGISPawn.h"

#include "MapCreator/ArcGISMapCreatorCameraTool.h"

class MapCreatorCameraToolDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization
	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	FReply AlignView(TObjectPtr<UArcGISMapCreatorCameraToolProperties> properties, TSharedRef<IPropertyUtilities> utilities);
	FReply CreateCamera(TObjectPtr<UArcGISMapCreatorCameraToolProperties> properties, TSharedRef<IPropertyUtilities> utilities);

	void CreateAlignButton(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorCameraToolProperties> properties);
	void CreateCameraButton(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorCameraToolProperties> properties);
	void SetPawnLocation(AArcGISPawn* pawn, const FGeoPosition& position, UArcGISRotation* rotation);
};
