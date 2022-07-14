// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#include "Security/SampleOAuthAuthenticationChallengeHandlerComponent.h"

#include "Engine/World.h"

#include "ArcGISMapsSDK/Subsystem/ArcGISMapsSDKSubsystem.h"
#include "Security/SampleOAuthAuthenticationChallengeHandler.h"

void USampleOAuthAuthenticationChallengeHandlerComponent::OnRegister()
{
	Super::OnRegister();

	SetChallengeHandler();
}

void USampleOAuthAuthenticationChallengeHandlerComponent::SetChallengeHandler()
{
	if (UWorld* OwningWorld = GetWorld(); OwningWorld && (OwningWorld->WorldType == EWorldType::PIE || OwningWorld->WorldType == EWorldType::Game))
	{
		if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
		{
			auto arcGISOAuthAuthenticationChallengeHandler = NewObject<USampleOAuthAuthenticationChallengeHandler>();

			arcGISMapsSDKSubsystem->SetAuthenticationChallengeHandler(arcGISOAuthAuthenticationChallengeHandler);
		}
	}
}
