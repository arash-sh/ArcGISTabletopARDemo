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

#include "MapCreator/CustomDetails/MapCreatorLayerToolDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"

#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

TSharedRef<IDetailCustomization> MapCreatorLayerToolDetails::MakeInstance()
{
	return MakeShared<MapCreatorLayerToolDetails>();
}

void MapCreatorLayerToolDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	MapComponent = MapComponentManager::GetCurrentMapComponent();

	TArray<TWeakObjectPtr<UObject>> object;
	DetailBuilder.GetObjectsBeingCustomized(object);
	TObjectPtr<UArcGISMapCreatorLayerToolProperties> properties = Cast<UArcGISMapCreatorLayerToolProperties>(object[0]);

	StyleAddNewData(DetailBuilder, properties);
}

// Add New Data

void MapCreatorLayerToolDetails::StyleAddNewData(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorLayerToolProperties> properties)
{
	const char* categoryName = "AddNewData";
	IDetailCategoryBuilder& addDataCategory = detailBuilder.EditCategory(categoryName);

	TArray<TSharedRef<IPropertyHandle>> categoryProperties;
	addDataCategory.GetDefaultProperties(categoryProperties);

	for (auto& prop : categoryProperties)
	{
		addDataCategory.GetParentLayout().HideProperty(prop);

		if (prop->GetProperty()->GetFName() == "LayerSource")
		{
			MapCreatorUtilities::MapCreatorWidgetFactory::CreateFileSelectorField(addDataCategory, categoryName, prop, &properties->LayerSource, nullptr);
		}
		else
		{
			addDataCategory.AddProperty(prop);
		}
	}

	addDataCategory.AddCustomRow(FText::FromString(categoryName))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		[
			SNew(SSpacer)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(SButton)
				.Content()
				[
					SNew(SBox)
					.Content()
					[
						SNew(STextBlock)
						.Text(FText::FromString("Clear"))
						.Justification(ETextJustify::Center)
						.Margin(FMargin(0, 2, 0, 0))
					]
				]
				.OnClicked(this, &MapCreatorLayerToolDetails::ClearFields, properties)
			]
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			.Padding(12, 0, 0, 0)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor(0.0f, 5.0f, 20.0f))
				.Content()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Add"))
					.Justification(ETextJustify::Center)
					.Margin(FMargin(0, 2, 0, 0))
				]
				.OnClicked(this, &MapCreatorLayerToolDetails::AddLayer, properties)
			]
		]
	];
}

FReply MapCreatorLayerToolDetails::ClearFields(TObjectPtr<UArcGISMapCreatorLayerToolProperties> properties)
{
	properties->LayerName = "";
	properties->LayerSource = "";
	properties->LayerType = EArcGISLayerType::ArcGISImageLayer;
	return FReply::Handled();
}

FReply MapCreatorLayerToolDetails::AddLayer(TObjectPtr<UArcGISMapCreatorLayerToolProperties> properties)
{
	// TODO(ssv-normandy): This check should include the layer type as in ArcGISMapComponent.cpp:CanCreateArcGISLayerFromDefinition
	if (properties->LayerSource == "" || !MapComponent)
	{
		return FReply::Handled();
	}

	FArcGISLayerInstanceData layer;
	{
		layer.Name = properties->LayerName;
		layer.Source = properties->LayerSource;
		layer.Type = properties->LayerType;
		layer.ID = FGuid::NewGuid();
	}
	properties->Layers.Add(layer);

	MapComponent->SetLayers(properties->Layers);

	ClearFields(properties);

	return FReply::Handled();
}
