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
#include "Widgets/Input/SCheckBox.h"

#include "MapCreator/ArcGISMapCreatorElevationTool.h"

class MapCreatorElevationToolDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization
	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TSharedPtr<SCheckBox> DefaultElevationCheckbox;
	TSharedPtr<SCheckBox> CustomElevationCheckbox;

	void SetDefaultCheckbox(ECheckBoxState state, TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties);
	void SetCustomCheckbox(ECheckBoxState state, TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties);
	TSharedPtr<SCheckBox> CreateCard(IDetailLayoutBuilder& detailBuilder,
					IDetailCategoryBuilder& category,
					FString categoryName,
					const FName& imageName,
					const FString& cardName,
					bool isChecked,
					TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties,
					void (MapCreatorElevationToolDetails::*func)(ECheckBoxState state, TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties));
};

