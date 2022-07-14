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

#include "MapCreator/ArcGISMapCreatorCameraTool.h"

#include "InteractiveToolManager.h"
#include "ToolBuilderUtil.h"

#include "ArcGISMapsSDK/Components/ArcGISCameraComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISLocationComponent.h"
#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

#define LOCTEXT_NAMESPACE "ArcGISMapCreatorCameraTool"

bool UArcGISMapCreatorCameraToolBuilder::CanBuildTool(const FToolBuilderState& SceneState) const
{
	return true;
}

UInteractiveTool* UArcGISMapCreatorCameraToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	UArcGISMapCreatorCameraTool* NewTool = NewObject<UArcGISMapCreatorCameraTool>(SceneState.ToolManager);
	return NewTool;
}

void UArcGISMapCreatorCameraToolBuilder::PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const
{
	UArcGISMapCreatorCameraTool* cameraTool = Cast<UArcGISMapCreatorCameraTool>(Tool);
	auto properties = cameraTool->Properties;

	auto World = GEditor->GetEditorWorldContext().World();
	UArcGISCameraComponent* camera = MapCreatorUtilities::CheckForComponent<UArcGISCameraComponent>(World);
	if (!camera)
	{
		return;
	}

	UArcGISLocationComponent* location = camera->GetOwner()->FindComponentByClass<UArcGISLocationComponent>();
	if (!location)
	{
		return;
	}

	auto position = location->GetPosition();
	auto rotation = location->GetRotation();

	properties->CameraPosition = position;

	//std::fmod(x, 360) wraps x between -360/360 so if the value is less than 360 just increment it to be 0/360
	auto heading = std::fmod(rotation->GetHeading(), 360);
	if (heading < 0)
	{
		heading += 360;
	}
	auto pitch = std::fmod(rotation->GetPitch(), 360);
	if (pitch < 0)
	{
		pitch += 360;
	}
	auto roll = std::fmod(rotation->GetRoll(), 360);
	if (roll < 0)
	{
		roll += 360;
	}
	properties->CameraRotation.CameraHeading = heading;
	properties->CameraRotation.CameraPitch = pitch;
	properties->CameraRotation.CameraRoll = roll;
}

void UArcGISMapCreatorCameraTool::Setup()
{
	UInteractiveTool::Setup();

	Properties = NewObject<UArcGISMapCreatorCameraToolProperties>(this);
	AddToolPropertySource(Properties);
}

void UArcGISMapCreatorCameraTool::OnPropertyModified(UObject* PropertySet, FProperty* Property)
{
	UInteractiveTool::OnPropertyModified(PropertySet, Property);

	if (!Property)
	{
		return;
	}

	auto World = GEditor->GetEditorWorldContext().World();

	UArcGISCameraComponent* camera = MapCreatorUtilities::CheckForComponent<UArcGISCameraComponent>(World);

	if (!camera)
	{
		return;
	}

	UArcGISLocationComponent* location = camera->GetOwner()->FindComponentByClass<UArcGISLocationComponent>();

	if (!location)
	{
		return;
	}

	auto position = location->GetPosition();

	if (FGeoPosition geoPosition(position); geoPosition != Properties->CameraPosition)
	{
		location->SetPosition(Properties->CameraPosition);
	}

	UArcGISRotation* rotation = location->GetRotation();

	if (Property->GetFName() == GET_MEMBER_NAME_CHECKED(FModesUICameraRotator, CameraHeading) ||
		Property->GetFName() == GET_MEMBER_NAME_CHECKED(FModesUICameraRotator, CameraPitch) ||
		Property->GetFName() == GET_MEMBER_NAME_CHECKED(FModesUICameraRotator, CameraRoll))
	{
		location->SetRotation(UArcGISRotation::CreateArcGISRotation(Properties->CameraRotation.CameraPitch, Properties->CameraRotation.CameraRoll, Properties->CameraRotation.CameraHeading));
	}

	auto mapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!mapComponent)
	{
		return;
	}

	mapComponent->MarkPackageDirty();
}

#undef LOCTEXT_NAMESPACE
