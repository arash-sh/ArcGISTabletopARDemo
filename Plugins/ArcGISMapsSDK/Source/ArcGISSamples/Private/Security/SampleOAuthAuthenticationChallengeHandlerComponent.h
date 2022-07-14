// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "SampleOAuthAuthenticationChallengeHandlerComponent.generated.h"

// This sample component demonstrates how to initialize an Authentication Challenge Handler for the Authentication Manager
// It can be attached to any object anywhere in your level
// You can and should create your own challenge handlers and component based on your own project's security needs
UCLASS(ClassGroup = (ArcGISSamples),
	   meta = (BlueprintSpawnableComponent),
	   Category = "ArcGISSamples",
	   hidecategories = (Activation, AssetUserData, Collision, Cooking, LOD, Object, Physics, Rendering, SceneComponent, Tags))
class USampleOAuthAuthenticationChallengeHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// UActorComponent
	void OnRegister() override;

	void SetChallengeHandler();
};
