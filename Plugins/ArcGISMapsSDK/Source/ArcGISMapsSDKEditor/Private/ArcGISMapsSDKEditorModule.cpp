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
#include "ArcGISMapsSDKEditorModule.h"

// ArcGISMapsSDKEditor

#include "ArcGISEditorModeCommands.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoPosition.h"
#include "ArcGISMapsSDK/Utils/AuthGuid.h"
#include "Assets/ArcGISAssetDirectory.h"
#include "DetailsPanel/ArcGISGeoPositionProperty.h"
#include "DetailsPanel/AuthGuidCustomization.h"
#include "DetailsPanel/ArcGISMapComponentExtentProperty.h"
#include "MapCreator/ArcGISMapCreatorLayerTool.h"
#include "MapCreator/ArcGISMapCreatorMapTool.h"
#include "MapCreator/CustomDetails/MapCreatorAuthenticationToolDetails.h"
#include "MapCreator/CustomDetails/MapCreatorBasemapToolDetails.h"
#include "MapCreator/CustomDetails/MapCreatorCameraToolDetails.h"
#include "MapCreator/CustomDetails/MapCreatorElevationToolDetails.h"
#include "MapCreator/CustomDetails/MapCreatorHelpToolDetails.h"
#include "MapCreator/CustomDetails/MapCreatorLayerToolDetails.h"
#include "MapCreator/CustomDetails/MapCreatorMapToolDetails.h"
#include "MapCreator/CustomDetails/MapCreatorStyles.h"

#define LOCTEXT_NAMESPACE "FArcGISMapsSDKEditorModule"

void FArcGISMapsSDKEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	{
		PropertyModule.RegisterCustomPropertyTypeLayout(
			FGeoPosition::StaticStruct()->GetFName(),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FGeoPositionDetailCustomization::MakeInstance));

		PropertyModule.RegisterCustomPropertyTypeLayout(
			FArcGISExtentInstanceData::StaticStruct()->GetFName(),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FMapComponentExtentPropertyCustomization::MakeInstance));

		PropertyModule.RegisterCustomPropertyTypeLayout(
			FAuthGuid::StaticStruct()->GetFName(),
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FAuthGuidCustomization::MakeInstance));

		PropertyModule.RegisterCustomClassLayout(UArcGISMapCreatorMapToolProperties::StaticClass()->GetFName(),
			FOnGetDetailCustomizationInstance::CreateStatic(&MapCreatorMapToolDetails::MakeInstance));

		PropertyModule.RegisterCustomClassLayout(UArcGISMapCreatorCameraToolProperties::StaticClass()->GetFName(),
												 FOnGetDetailCustomizationInstance::CreateStatic(&MapCreatorCameraToolDetails::MakeInstance));

		PropertyModule.RegisterCustomClassLayout(UArcGISMapCreatorLayerToolProperties::StaticClass()->GetFName(),
												 FOnGetDetailCustomizationInstance::CreateStatic(&MapCreatorLayerToolDetails::MakeInstance));

		PropertyModule.RegisterCustomClassLayout(UArcGISMapCreatorBasemapToolProperties::StaticClass()->GetFName(),
												 FOnGetDetailCustomizationInstance::CreateStatic(&MapCreatorBasemapToolDetails::MakeInstance));

		PropertyModule.RegisterCustomClassLayout(UArcGISMapCreatorElevationToolProperties::StaticClass()->GetFName(),
												 FOnGetDetailCustomizationInstance::CreateStatic(&MapCreatorElevationToolDetails::MakeInstance));

		PropertyModule.RegisterCustomClassLayout(UArcGISMapCreatorAuthenticationToolProperties::StaticClass()->GetFName(),
												 FOnGetDetailCustomizationInstance::CreateStatic(&MapCreatorAuthenticationToolDetails::MakeInstance));
	
		PropertyModule.RegisterCustomClassLayout(UArcGISMapCreatorHelpToolProperties::StaticClass()->GetFName(),
												 FOnGetDetailCustomizationInstance::CreateStatic(&MapCreatorHelpToolDetails::MakeInstance));
	}

	FArcGISAssetDirectory::LoadForCook();
	FArcGISEditorModeCommands::Register();
}

void FArcGISMapsSDKEditorModule::ShutdownModule()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		{
			PropertyModule.UnregisterCustomClassLayout(UArcGISMapCreatorLayerToolProperties::StaticClass()->GetFName());
			PropertyModule.UnregisterCustomClassLayout(UArcGISMapCreatorBasemapToolProperties::StaticClass()->GetFName());
			PropertyModule.UnregisterCustomClassLayout(UArcGISMapCreatorHelpToolProperties::StaticClass()->GetFName());
		}
	}

	MapCreatorStyles::Shutdown();
	FArcGISAssetDirectory::ReleaseAll();
	FArcGISEditorModeCommands::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FArcGISMapsSDKEditorModule, ArcGISMapsSDKEditor)
