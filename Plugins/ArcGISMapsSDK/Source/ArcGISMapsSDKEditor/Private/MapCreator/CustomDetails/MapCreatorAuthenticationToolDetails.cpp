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

#include "MapCreator/CustomDetails/MapCreatorAuthenticationToolDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"

#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

TSharedRef<IDetailCustomization> MapCreatorAuthenticationToolDetails::MakeInstance()
{
	return MakeShared<MapCreatorAuthenticationToolDetails>();
}

void MapCreatorAuthenticationToolDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	MapComponent = MapComponentManager::GetCurrentMapComponent();

	TArray<TWeakObjectPtr<UObject>> object;
	DetailBuilder.GetObjectsBeingCustomized(object);
	TObjectPtr<UArcGISMapCreatorAuthenticationToolProperties> properties = Cast<UArcGISMapCreatorAuthenticationToolProperties>(object[0]);

	StyleAddNewData(DetailBuilder, properties);
}

// Add New Data

void MapCreatorAuthenticationToolDetails::StyleAddNewData(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorAuthenticationToolProperties> properties)
{
	auto apiProperty = detailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UArcGISMapCreatorAuthenticationToolProperties, APIKey));
	detailBuilder.AddPropertyToCategory(apiProperty);

	const char* categoryName = "AddAuthenticationConfiguration";
	IDetailCategoryBuilder& AddDataCategory = detailBuilder.EditCategory(categoryName);

	AddDataCategory.AddCustomRow(FText::FromString(categoryName))
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
				.OnClicked(this, &MapCreatorAuthenticationToolDetails::ClearFields, properties)
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
				.OnClicked(this, &MapCreatorAuthenticationToolDetails::AddLayer, properties)
			]
		]
	];
}

FReply MapCreatorAuthenticationToolDetails::ClearFields(TObjectPtr<UArcGISMapCreatorAuthenticationToolProperties> properties)
{
	properties->Name = "";
	properties->ClientID = "";
	properties->RedirectURI = "";
	return FReply::Handled();
}

FReply MapCreatorAuthenticationToolDetails::AddLayer(TObjectPtr<UArcGISMapCreatorAuthenticationToolProperties> properties)
{
	if (properties->ClientID.IsEmpty() || properties->RedirectURI.IsEmpty() || !MapComponent)
	{
		return FReply::Handled();
	}

	FArcGISAuthenticationConfigurationInstanceData config;
	{
		config.Name = properties->Name;
		config.ClientID = properties->ClientID;
		config.RedirectURI = properties->RedirectURI;
	}

	properties->Configs.Add(config);

	MapComponent->SetAuthenticationConfigurations(properties->Configs);

	ClearFields(properties);

	return FReply::Handled();
}
