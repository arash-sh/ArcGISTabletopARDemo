// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#pragma once

//#include "Components/SceneComponent.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SampleArcGISRaycast.generated.h"

UCLASS(ClassGroup = (ArcGISSamples), meta = (BlueprintSpawnableComponent), Category = "ArcGISSamples|SampleArcGISRaycast")
class ARCGISSAMPLES_API USampleArcGISRaycast : public UActorComponent
{
	GENERATED_BODY()

public:
	USampleArcGISRaycast();

	UFUNCTION(BlueprintCallable, Category = "ArcGISSamples|SampleArcGISRaycast")
	bool GetHitResultUnderCursor(ECollisionChannel TraceChannel, FHitResult& HitResult);
};
