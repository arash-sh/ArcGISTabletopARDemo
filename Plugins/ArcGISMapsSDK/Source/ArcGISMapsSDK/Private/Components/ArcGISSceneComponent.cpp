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
#include "ArcGISMapsSDK/Components/ArcGISSceneComponent.h"

#include "ArcGISMapsSDK/Subsystem/ArcGISMapsSDKSubsystem.h"

#if WITH_EDITORONLY_DATA
bool UArcGISSceneComponent::IsInEditorPreviewActor() const
{
	auto Owner = GetOwner();

	// Owner->bIsEditorPreviewActor should be used instead but Unreal is currently not setting it
	// https://ue5help.epicgames.com/s/question/0D54z00007Y4IW4CAN/biseditorpreviewactor-false-for-actors-being-dragged
	return !Owner || Owner->HasAnyFlags(RF_Transient);
}
#endif

void UArcGISSceneComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITORONLY_DATA
	if (!IsInEditorPreviewActor())
#endif
	{
		if (auto OwningWorld = GetWorld(); OwningWorld && OwningWorld->WorldType != EWorldType::Inactive)
		{
			if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
			{
				arcGISMapsSDKSubsystem->RegisterListener(this);
			}
		}
	}
}

void UArcGISSceneComponent::OnUnregister()
{
#if WITH_EDITORONLY_DATA
	if (!IsInEditorPreviewActor())
#endif
	{
		if (auto OwningWorld = GetWorld(); OwningWorld && OwningWorld->WorldType != EWorldType::Inactive)
		{
			if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
			{
				arcGISMapsSDKSubsystem->UnregisterListener(this);
			}
		}
	}

	Super::OnUnregister();
}

void UArcGISSceneComponent::OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent)
{
	if (MapComponent != InMapComponent)
	{
		MapComponent = InMapComponent;
	}
}
