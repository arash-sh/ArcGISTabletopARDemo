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
#include "MapCreator/CustomDetails/MapCreatorStyles.h"

#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Slate/SlateGameResources.h"
#include "Styling/SlateStyleRegistry.h"

#include "ArcGISMapsSDKEditorModule.h"

#define PNGImageBrush(RelativePath, ...) FSlateImageBrush(StyleSet->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

const FVector2D BaseMapMargin(400, 65.67f);
const FVector2D LogoMargin(370, 114);
const FVector2D SmallIconMargin(20, 20);

TSharedPtr<FSlateStyleSet> MapCreatorStyles::CommonStyleSet = nullptr;

void CreateHyperlinkButtonStyle(TSharedPtr<FSlateStyleSet> StyleSet)
{
	const FButtonStyle hyperlinkButtonStyle = FButtonStyle()
		.SetNormal(FSlateNoResource())
		.SetHovered(FSlateNoResource())
		.SetPressed(FSlateNoResource());

	auto defaultText = FCoreStyle::Get().GetWidgetStyle<FHyperlinkStyle>("Hyperlink").TextStyle;

	FTextBlockStyle hyperlinkTextStyle = FTextBlockStyle(defaultText)
		.SetColorAndOpacity(FColor(0, 43, 170));

	const FHyperlinkStyle hyperlinkStyle = FHyperlinkStyle()
		.SetPadding(FMargin(0, 4, 0, 4))
		.SetTextStyle(hyperlinkTextStyle)
		.SetUnderlineStyle(hyperlinkButtonStyle);

	StyleSet->Set("HyperlinkStyle", hyperlinkStyle);
}

TSharedRef<FSlateStyleSet> MapCreatorStyles::CreateCommonStyleSet()
{
	TSharedRef<FSlateStyleSet> StyleSet = MakeShareable(new FSlateStyleSet("MapCreatorStyles"));

	auto pluginDir = IPluginManager::Get().FindPlugin("ArcGISMapsSDK")->GetBaseDir();

	StyleSet->SetContentRoot(pluginDir / TEXT("Resources"));

	StyleSet->Set("EsriLogo", new PNGImageBrush(TEXT("ESRI_LOGO"), LogoMargin));

	StyleSet->SetContentRoot(pluginDir / TEXT("Resources/BaseMaps"));

	StyleSet->Set("Imagery", new PNGImageBrush(TEXT("IMAGERY"), BaseMapMargin));
	StyleSet->Set("Oceans", new PNGImageBrush(TEXT("OCEANS"), BaseMapMargin));
	StyleSet->Set("Custom", new PNGImageBrush(TEXT("CUSTOM"), BaseMapMargin));

	StyleSet->SetContentRoot(pluginDir / TEXT("Resources/ElevationCards"));

	StyleSet->Set("ElevationDefault", new PNGImageBrush(TEXT("elevation_01_default"), BaseMapMargin));
	StyleSet->Set("ElevationCustom", new PNGImageBrush(TEXT("elevation_02_custom"), BaseMapMargin));

	StyleSet->SetContentRoot(pluginDir / TEXT("Resources/EditorModeIcons"));

	StyleSet->Set("ArcGISMiniIcon", new PNGImageBrush(TEXT("arcgis-maps-sdks-24"), SmallIconMargin));
	StyleSet->Set("MapIcon", new PNGImageBrush(TEXT("new_map_icon"), SmallIconMargin));
	StyleSet->Set("CameraIcon", new PNGImageBrush(TEXT("camera_icon"), SmallIconMargin));
	StyleSet->Set("BasemapIcon", new PNGImageBrush(TEXT("basemap_icon"), SmallIconMargin));
	StyleSet->Set("ElevationIcon", new PNGImageBrush(TEXT("elevation_icon"), SmallIconMargin));
	StyleSet->Set("LayersIcon", new PNGImageBrush(TEXT("layers_icon"), SmallIconMargin));
	StyleSet->Set("AuthIcon", new PNGImageBrush(TEXT("authentication_icon"), SmallIconMargin));
	StyleSet->Set("SettingsIcon", new PNGImageBrush(TEXT("settings_icon"), SmallIconMargin));
	StyleSet->Set("HelpIcon", new PNGImageBrush(TEXT("help_icon"), SmallIconMargin));

	CreateHyperlinkButtonStyle(StyleSet);

	return StyleSet;
}

const FSlateStyleSet* MapCreatorStyles::GetCommonStyleSet()
{
	return CommonStyleSet.Get();
}

void MapCreatorStyles::Initialize()
{
	if (!CommonStyleSet.IsValid())
	{
		CommonStyleSet = CreateCommonStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CommonStyleSet);
	}

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

void MapCreatorStyles::Shutdown()
{
	UnregisterStyle(CommonStyleSet);
}

void MapCreatorStyles::UnregisterStyle(TSharedPtr<FSlateStyleSet> Style)
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*Style);
	Style.Reset();
}

#undef PNGImageBrush
