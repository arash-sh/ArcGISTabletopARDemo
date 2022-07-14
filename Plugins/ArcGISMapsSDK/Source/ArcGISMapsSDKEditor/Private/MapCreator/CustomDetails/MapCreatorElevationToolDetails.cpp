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
#include "MapCreator/CustomDetails/MapCreatorElevationToolDetails.h"

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

TSharedRef<IDetailCustomization> MapCreatorElevationToolDetails::MakeInstance()
{
	return MakeShared<MapCreatorElevationToolDetails>();
}

void MapCreatorElevationToolDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	auto map = MapComponentManager::GetCurrentMapComponent();

	TArray<TWeakObjectPtr<UObject>> object;
	DetailBuilder.GetObjectsBeingCustomized(object);
	TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties = Cast<UArcGISMapCreatorElevationToolProperties>(object[0]);

	constexpr char categoryName[] = "Elevation";
	auto& category = DetailBuilder.EditCategory(categoryName);

	DefaultElevationCheckbox = CreateCard(DetailBuilder, category, categoryName, "ElevationDefault", "Default Elevation",
			   properties->IsDefaultElevationSource(), properties, &MapCreatorElevationToolDetails::SetDefaultCheckbox);
	CustomElevationCheckbox = CreateCard(DetailBuilder, category, categoryName, "ElevationCustom", "Custom Elevation",
			   properties->IsCustomElevationSource(), properties, &MapCreatorElevationToolDetails::SetCustomCheckbox);

	auto fileSelectCallback = MakeShared<FSimpleDelegate>(FSimpleDelegate::CreateLambda([map, properties]()
	{
		if (!map)
		{
			return;
		}

		if (map->GetIsElevationEnabled() && map->GetElevationSource() != properties->CustomElevationSource && properties->EnableCustomElevation)
		{
			map->SetElevationSource(properties->CustomElevationSource);
		}
	}));

	auto elevationSourceProperty = DetailBuilder.GetProperty("CustomElevationSource");
	DetailBuilder.HideProperty(elevationSourceProperty);
	MapCreatorUtilities::MapCreatorWidgetFactory::CreateFileSelectorField(category, categoryName, elevationSourceProperty, &properties->CustomElevationSource, fileSelectCallback);

	auto elevationAuthentication = DetailBuilder.GetProperty("ElevationAuthConfigID");
	DetailBuilder.HideProperty(elevationAuthentication);
	category.AddProperty(elevationAuthentication);
}

void MapCreatorElevationToolDetails::SetDefaultCheckbox(ECheckBoxState state, TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties)
{
	bool isChecked = state == ECheckBoxState::Checked ? true : false;
	properties->SetDefaultElevation(isChecked);
	properties->EnableCustomElevation = false;
	CustomElevationCheckbox->SetIsChecked(false);
}

void MapCreatorElevationToolDetails::SetCustomCheckbox(ECheckBoxState state, TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties)
{
	bool isChecked = state == ECheckBoxState::Checked ? true : false;
	properties->SetCustomElevation(isChecked);
	properties->EnableCustomElevation = isChecked;
	DefaultElevationCheckbox->SetIsChecked(false);
}

TSharedPtr<SCheckBox> MapCreatorElevationToolDetails::CreateCard(IDetailLayoutBuilder& detailBuilder,
												IDetailCategoryBuilder& category,
												FString categoryName,
												const FName& imageName,
												const FString& cardName,
												bool isChecked,
												TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties,
												void (MapCreatorElevationToolDetails::*func)(ECheckBoxState state, TObjectPtr<UArcGISMapCreatorElevationToolProperties> properties))
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
						.Image(MapCreatorStyles::GetCommonStyleSet()->GetBrush(imageName))
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
						.IsChecked(isChecked)
						.OnCheckStateChanged(this, func, properties)
					]
					+ SHorizontalBox::Slot()
					.Padding(FMargin(0, 3))
					[
						SNew(STextBlock)
						.Text(FText::FromString(cardName))
						.Font(detailBuilder.GetDetailFont())
						.Justification(ETextJustify::Left)
						.Margin(FMargin(2, 4, 0, 0))
					]
				]
			]
		]
	];

	return checkbox;
}
