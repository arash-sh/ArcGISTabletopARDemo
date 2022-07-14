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
#include "MapCreator/CustomDetails/MapCreatorBasemapToolDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Layout/SScaleBox.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "MapCreator/ArcGISMapCreatorUtilities.h"
#include "MapCreator/CustomDetails/MapCreatorStyles.h"

using namespace MapCreatorUtilities;

static const TArray<FString> Labels =
{
	TEXT("World Imagery"),
	TEXT("Oceans"),
	TEXT("Custom")
};

TSharedRef<IDetailCustomization> MapCreatorBasemapToolDetails::MakeInstance()
{
	return MakeShared<MapCreatorBasemapToolDetails>();
}

void MapCreatorBasemapToolDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto map = MapComponentManager::GetCurrentMapComponent();

	TArray<TWeakObjectPtr<UObject>> object;
	DetailBuilder.GetObjectsBeingCustomized(object);
	TObjectPtr<UArcGISMapCreatorBasemapToolProperties> properties = Cast<UArcGISMapCreatorBasemapToolProperties>(object[0]);

	DetailBuilder.HideCategory("Basemap");

	const char* categoryName = "Basemaps";
	auto& category = DetailBuilder.EditCategory(categoryName);

	for (int i = 0; i < Labels.Num(); i++)
	{
		TSharedPtr<SCheckBox> checkbox;

		category.AddCustomRow(FText::FromString(categoryName))
		.IsValueEnabled(false)
		.WholeRowContent()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		[
			SNew(SBox)
			.Visibility(EVisibility::SelfHitTestInvisible)
			.Padding(FMargin(0, 10))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1)
				.AutoHeight()
				.VAlign(EVerticalAlignment::VAlign_Fill)
				[
					SNew(SBox)
					.HeightOverride(66)
					[
						SNew(SScaleBox)
						.Stretch(EStretch::ScaleToFill)
						[
							SNew(SImage)
							.Image(GetBaseMapImage(i))
						]
					]
				]
				+ SVerticalBox::Slot()
				.MaxHeight(26)
				.AutoHeight()
				[
					SNew(SOverlay)
					+ SOverlay::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Fill)
					.VAlign(EVerticalAlignment::VAlign_Fill)
					[
						SNew(SImage)
						.Image(new FSlateBrush())
						.ColorAndOpacity(FSlateColor(FLinearColor(0.05f, 0.05f, 0.05f, 1.0f)))
					]
					+ SOverlay::Slot()
					.Padding(8, 0, 0, 0)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(0.1f)
						[
							SAssignNew(checkbox, SCheckBox)
							.IsChecked(GetCheckboxState(Labels[i], properties->SelectedBasemap))
							.OnCheckStateChanged_Lambda([this, properties, map, i](ECheckBoxState state)
							{
								for (int index = 0; index < CheckboxList.Num(); index++)
								{
									CheckboxList[index]->SetIsChecked(false);
								}

								if (state == ECheckBoxState::Checked)
								{
									CheckboxList[i]->SetIsChecked(true);
									properties->SelectedBasemap = (EDefaultBaseMaps)i;

									if (properties->SelectedBasemap == EDefaultBaseMaps::Custom)
									{
										UArcGISMapCreatorBasemapTool::SetCustomBasemap(map, properties->CustomBasemapSource, properties->CustomBasemapType);
									}
									else
									{
										UArcGISMapCreatorBasemapTool::SetDefaultBasemap(map, properties->SelectedBasemap);
									}
								}
								else
								{
									properties->SelectedBasemap = EDefaultBaseMaps::NoBasemapType;
									UArcGISMapCreatorBasemapTool::SetCustomBasemap(map, "", properties->CustomBasemapType);
								}
							})
						]
						+ SHorizontalBox::Slot()
						.Padding(FMargin(0, 3))
						[
							SNew(STextBlock)
							.Text(FText::FromString(Labels[i]))
							.Font(DetailBuilder.GetDetailFont())
							.Justification(ETextJustify::Left)
							.Margin(FMargin(2, 4, 0, 0))
						]
					]
				]
			]
		];

		CheckboxList.Add(checkbox);
	}

	auto callback = MakeShared<FSimpleDelegate>(FSimpleDelegate::CreateLambda([properties, map]()
	{
		if (properties->SelectedBasemap == EDefaultBaseMaps::Custom)
		{
			UArcGISMapCreatorBasemapTool::SetCustomBasemap(map, properties->CustomBasemapSource, properties->CustomBasemapType);
		}
	}));

	auto basemapSourceProperty = DetailBuilder.GetProperty("CustomBasemapSource");
	MapCreatorUtilities::MapCreatorWidgetFactory::CreateFileSelectorField(category, categoryName, basemapSourceProperty, &properties->CustomBasemapSource, callback);

	auto basemapType = DetailBuilder.GetProperty("CustomBasemapType");
	category.AddProperty(basemapType);

	auto basemapAuthentication = DetailBuilder.GetProperty("BasemapAuthConfigID");
	category.AddProperty(basemapAuthentication);

	if (!map)
	{
		return;
	}

	basemapType->SetValue((int)map->GetBasemapType());
}

const FSlateBrush* MapCreatorBasemapToolDetails::GetBaseMapImage(const int index)
{
	auto BaseMap = (EDefaultBaseMaps)index;

	switch (BaseMap)
	{
		case Imagery:
			return MapCreatorStyles::GetCommonStyleSet()->GetBrush("Imagery");
		case Oceans:
			return MapCreatorStyles::GetCommonStyleSet()->GetBrush("Oceans");
		default:
			return MapCreatorStyles::GetCommonStyleSet()->GetBrush("Custom");
	}
}

bool MapCreatorBasemapToolDetails::GetCheckboxState(const FString& label, EDefaultBaseMaps basemap)
{
	if (basemap == EDefaultBaseMaps::Imagery && label == Labels[0])
	{
		return true;
	}

	if (basemap == EDefaultBaseMaps::Oceans && label == Labels[1])
	{
		return true;
	}

	if (basemap == EDefaultBaseMaps::Custom && label == Labels[2])
	{
		return true;
	}

	if (basemap == EDefaultBaseMaps::NoBasemapType)
	{
		return false;
	}

	return false;
}
