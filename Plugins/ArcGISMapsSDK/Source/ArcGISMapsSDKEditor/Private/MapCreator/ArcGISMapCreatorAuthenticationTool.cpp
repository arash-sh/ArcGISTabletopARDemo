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

#include "MapCreator/ArcGISMapCreatorAuthenticationTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

#define LOCTEXT_NAMESPACE "ArcGISMapCreatorAuthenticationTool"

bool UArcGISMapCreatorAuthenticationToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UArcGISMapCreatorAuthenticationToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	return NewObject<UArcGISMapCreatorAuthenticationTool>(SceneState.ToolManager);
}

void UArcGISMapCreatorAuthenticationToolBuilder::PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const
{
	UArcGISMapCreatorAuthenticationTool* authTool = Cast<UArcGISMapCreatorAuthenticationTool>(Tool);
	auto properties = authTool->Properties;

	auto mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!mapComponent)
	{
		return;
	}

	properties->APIKey = mapComponent->GetAPIkey();
	properties->Configs = mapComponent->GetAuthenticationConfigurations();
}

void UArcGISMapCreatorAuthenticationTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UArcGISMapCreatorAuthenticationToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UArcGISMapCreatorAuthenticationTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	auto mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!mapComponent)
	{
		return;
	}

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UArcGISMapCreatorAuthenticationToolProperties, APIKey))
	{
		mapComponent->SetAPIkey(Properties->APIKey);
	}
	else if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(UArcGISMapCreatorAuthenticationToolProperties, Configs) ||
			 Property->GetFName() == GET_MEMBER_NAME_CHECKED(FArcGISAuthenticationConfigurationInstanceData, Name) ||
			 Property->GetFName() == GET_MEMBER_NAME_CHECKED(FArcGISAuthenticationConfigurationInstanceData, ID) ||
			 Property->GetFName() == GET_MEMBER_NAME_CHECKED(FArcGISAuthenticationConfigurationInstanceData, ClientID) ||
			 Property->GetFName() == GET_MEMBER_NAME_CHECKED(FArcGISAuthenticationConfigurationInstanceData, RedirectURI))
	{
		mapComponent->SetAuthenticationConfigurations(Properties->Configs);
	}
}

#undef LOCTEXT_NAMESPACE
