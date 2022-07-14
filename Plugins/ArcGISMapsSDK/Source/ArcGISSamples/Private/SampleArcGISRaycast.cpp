// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#include "SampleArcGISRaycast.h"

#include "CollisionQueryParams.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

USampleArcGISRaycast::USampleArcGISRaycast()
{
	PrimaryComponentTick.bCanEverTick = false;
	bTickInEditor = false;
	bAutoActivate = true;
}

bool USampleArcGISRaycast::GetHitResultUnderCursor(ECollisionChannel TraceChannel, FHitResult& HitResult)
{
	UWorld* world = GetWorld();
	APlayerController* PlayerController = Cast<APlayerController>(world->GetFirstPlayerController());
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->Player);
	bool bHit = false;
	if (LocalPlayer && LocalPlayer->ViewportClient)
	{
		FVector2D MousePosition;
		if (LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
		{
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.bTraceComplex = true;
			CollisionQueryParams.bReturnFaceIndex = true;

			bHit = PlayerController->GetHitResultAtScreenPosition(MousePosition, TraceChannel, CollisionQueryParams, HitResult);
		}
	}
	return bHit;
}
