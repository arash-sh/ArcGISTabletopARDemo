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

#include "MapCreator/ArcGISMapCreatorMapTool.h"

class MapCreatorMapToolDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TSharedPtr<SButton> GlobalButton;
	TSharedPtr<SButton> LocalButton;

	FReply CreateMap(TObjectPtr<UArcGISMapCreatorMapToolProperties> properties, TSharedRef<IPropertyUtilities> utilities);
	FReply SetMapType(bool isGlobal, TObjectPtr<UArcGISMapCreatorMapToolProperties> properties);

	void StyleMapTool(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorMapToolProperties> properties);
	void CreateLogo(IDetailLayoutBuilder& detailBuilder);
	void CreateMapTypeButton(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorMapToolProperties> properties);
	void CreateMapButton(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorMapToolProperties> properties);
};
