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
#include "MapCreator/CustomDetails/MapCreatorMapToolDetails.h"

#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IPropertyUtilities.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/STextComboBox.h"
#include "Widgets/Layout/SScaleBox.h"

#include "ArcGISMapsSDK/Actors/ArcGISMapActor.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMapType.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoPosition.h"
#include "MapCreator/ArcGISMapCreatorUtilities.h"
#include "MapCreator/CustomDetails/MapCreatorStyles.h"

using namespace MapCreatorUtilities;

TSharedRef<IDetailCustomization> MapCreatorMapToolDetails::MakeInstance()
{
	return MakeShared<MapCreatorMapToolDetails>();
}

void MapCreatorMapToolDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> object;
	DetailBuilder.GetObjectsBeingCustomized(object);
	TObjectPtr<UArcGISMapCreatorMapToolProperties> properties = Cast<UArcGISMapCreatorMapToolProperties>(object[0]);

	StyleMapTool(DetailBuilder, properties);
}

// Add New Data

void MapCreatorMapToolDetails::StyleMapTool(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorMapToolProperties> properties)
{
	auto world = GEditor->GetEditorWorldContext().World();
	AArcGISMapActor* mapActor = Cast<AArcGISMapActor>(UGameplayStatics::GetActorOfClass(world, AArcGISMapActor::StaticClass()));

	CreateLogo(detailBuilder);
	CreateMapTypeButton(detailBuilder, properties);
	CreateMapButton(detailBuilder, properties);
}

void MapCreatorMapToolDetails::CreateLogo(IDetailLayoutBuilder& detailBuilder)
{
	constexpr const char* logoCategoryName = "ArcGIS Maps SDK for Unreal Engine";

	auto& logoCategory = detailBuilder.EditCategory(logoCategoryName);
	// clang-format off
	logoCategory.AddCustomRow(FText::FromString(logoCategoryName))
	[
		SNew(SScaleBox)
		[
			SNew(SImage)
			.Image(MapCreatorStyles::GetCommonStyleSet()->GetBrush("EsriLogo"))
		]
	];
	// clang-format on
}

void MapCreatorMapToolDetails::CreateMapTypeButton(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorMapToolProperties> properties)
{
	constexpr const char* mapTypeCategoryName = "MapType";

	detailBuilder.HideProperty(mapTypeCategoryName);

	auto& mapTypeCategory = detailBuilder.EditCategory(mapTypeCategoryName);

	// clang-format off
	mapTypeCategory.AddCustomRow(FText::FromString(mapTypeCategoryName))
	.WholeRowContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SSpacer)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(2)
		[
			SAssignNew(GlobalButton, SButton)
			.OnClicked(this, &MapCreatorMapToolDetails::SetMapType, true, properties)
			.Content()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Global"))
				.Justification(ETextJustify::Center)
				.Margin(FMargin(0, 2, 0, 0))
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(2)
		[
			SAssignNew(LocalButton, SButton)
			.OnClicked(this, &MapCreatorMapToolDetails::SetMapType, false, properties)
			.Content()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Local"))
				.Justification(ETextJustify::Center)
				.Margin(FMargin(0, 2, 0, 0))
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SSpacer)
		]
	];
	// clang-format on

	GlobalButton->SetBorderBackgroundColor(properties->MapType == EViewMode::Global ? FLinearColor::Black : FLinearColor::Gray);
	LocalButton->SetBorderBackgroundColor(properties->MapType == EViewMode::Local ? FLinearColor::Black : FLinearColor::Gray);
}

FReply MapCreatorMapToolDetails::SetMapType(bool isGlobal, TObjectPtr<UArcGISMapCreatorMapToolProperties> properties)
{
	properties->MapType = isGlobal ? EViewMode::Global : EViewMode::Local;

	GlobalButton->SetEnabled(isGlobal ? false : true);
	LocalButton->SetEnabled(isGlobal ? true : false);

	GlobalButton->SetBorderBackgroundColor(isGlobal ? FLinearColor::Black : FLinearColor::Gray);
	LocalButton->SetBorderBackgroundColor(isGlobal ? FLinearColor::Gray : FLinearColor::Black);

	auto mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (mapComponent)
	{
		mapComponent->SetMapType(isGlobal ? EArcGISMapType::Global : EArcGISMapType::Local);
	}

	return FReply::Handled();
}

void MapCreatorMapToolDetails::CreateMapButton(IDetailLayoutBuilder& detailBuilder, TObjectPtr<UArcGISMapCreatorMapToolProperties> properties)
{
	constexpr const char* mapCategoryName = "Create Map";
	auto& createMapButtonCategory = detailBuilder.EditCategory(mapCategoryName);
	auto propUtilities = detailBuilder.GetPropertyUtilities();
	auto buttonColor = FLinearColor(0.0f, 5.0f, 20.0f);
	auto mapActor = MapComponentManager::GetCurrentMapComponent();

	if (mapActor)
	{
		buttonColor = FLinearColor::Gray;
	}

	// clang-format off
	createMapButtonCategory.AddCustomRow(FText::FromString(mapCategoryName))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		[
			SNew(SSpacer)
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(buttonColor)
				.Content()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Create"))
					.Justification(ETextJustify::Center)
					.Margin(FMargin(0, 2, 0, 0))
				]
				.OnClicked(this, &MapCreatorMapToolDetails::CreateMap, properties, propUtilities)
				.IsEnabled(!mapActor)
			]
		]
	];
	// clang-format on

	auto& originCategory = detailBuilder.EditCategory("OriginPosition");
	auto& extentCategory = detailBuilder.EditCategory("MapExtent");

	extentCategory.SetSortOrder(originCategory.GetSortOrder() + 1);
	createMapButtonCategory.SetSortOrder(originCategory.GetSortOrder() + 2);
}

FReply MapCreatorMapToolDetails::CreateMap(TObjectPtr<UArcGISMapCreatorMapToolProperties> properties, TSharedRef<IPropertyUtilities> utilities)
{
	auto world = GEditor->GetEditorWorldContext().World();

	AArcGISMapActor* mapActor = Cast<AArcGISMapActor>(UGameplayStatics::GetActorOfClass(world, AArcGISMapActor::StaticClass()));

	if (mapActor)
	{
		return FReply::Handled();
	}

	mapActor = world->SpawnActor<AArcGISMapActor>();
	auto mapComponent = mapActor->GetMapComponent();
	MapComponentManager::SetCurrentMapComponent(mapComponent);

	mapComponent->SetOriginPosition(properties->OriginPosition);
	mapComponent->SetIsOriginAtPlanetCenter(properties->bIsOriginAtPlanetCenter);

	mapComponent->SetExtent(properties->MapExtent);
	EArcGISMapType::Type mapType = properties->MapType == EViewMode::Global ? EArcGISMapType::Global : EArcGISMapType::Local;
	mapComponent->SetMapType(mapType);

	//Default values for other tabs
	{
		mapComponent->SetIsElevationEnabled(true);
		mapComponent->SetElevationSource(TEXT("https://elevation3d.arcgis.com/arcgis/rest/services/WorldElevation3D/Terrain3D/ImageServer"));
		mapComponent->SetBasemapSourceAndType(TEXT("https://ibasemaps-api.arcgis.com/arcgis/rest/services/World_Imagery/MapServer"),
											  EBaseMapTypes::ImageLayer);
	}

	utilities->ForceRefresh();

	return FReply::Handled();
}
