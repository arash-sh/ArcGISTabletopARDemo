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
#include "Subsystem/ArcGISMapsSDKEditorSubsystem.h"

// Unreal

#include "Engine/World.h"

// ArcGISMapsSDK

#include "ArcGISMapsSDK/Subsystem/ArcGISMapsSDKSubsystem.h"

// ArcGISMapsSDKEditor

#include "Security/ArcGISEditorOAuthAuthenticationChallengeHandler.h"

void UArcGISMapsSDKEditorSubsystem::PostInitialize()
{
	if (UWorld* OwningWorld = GetWorld(); OwningWorld->WorldType == EWorldType::Editor)
	{
		if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
		{
			auto arcGISOAuthAuthenticationChallengeHandler = NewObject<UArcGISEditorOAuthAuthenticationChallengeHandler>();

			arcGISMapsSDKSubsystem->SetAuthenticationChallengeHandler(arcGISOAuthAuthenticationChallengeHandler);
		}
	}
}
