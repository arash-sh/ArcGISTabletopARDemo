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

#include "MapCreator/ArcGISMapCreatorBasemapTool.h"

class MapCreatorBasemapToolDetails : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();
	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:
	TArray<TSharedPtr<SCheckBox>> CheckboxList;

	bool GetCheckboxState(const FString& label, EDefaultBaseMaps basemap);
	const FSlateBrush* GetBaseMapImage(const int index);
};

