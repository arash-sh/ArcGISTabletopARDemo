// COPYRIGHT 1995-2021 ESRI
// TRADE SECRETS: ESRI PROPRIETARY AND CONFIDENTIAL
// Unpublished material - all rights reserved under the
// Copyright Laws of the United States and applicable international
// laws, treaties, and conventions.
//
// For additional information, contact:
// Environmental Systems Research Institute, Inc.
// Attn: Contracts and Legal Services Department
// 380 New York Street
// Redlands, California, 92373
// USA
//
// email: contracts@esri.com

#include "ArcGISMapsSDK/Utils/Unreal/CameraHelpers.h"

#if WITH_EDITOR
#include "EditorViewportClient.h"
#endif

#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

FVector UCameraHelpers::GetActiveCameraPosition(const UObject* InWorldContextObject)
{
	FVector CameraPosition{0, 0, 0};

#if WITH_EDITOR
	if (InWorldContextObject->GetWorld()->WorldType == EWorldType::Editor)
	{
		if (auto ActiveViewport = GEditor->GetActiveViewport())
		{
			if (auto EditorViewClient = static_cast<FEditorViewportClient*>(ActiveViewport->GetClient()))
			{
				CameraPosition = EditorViewClient->GetViewLocation();
			}
		}
	}
	else
#endif
	{
		if (const auto& Controller = InWorldContextObject->GetWorld()->GetFirstPlayerController())
		{
			CameraPosition = Controller->PlayerCameraManager->GetCameraLocation();
		}
	}

	return CameraPosition;
}

FMatrix UCameraHelpers::GetActiveCameraViewProjMatrix(const UObject* InWorldContextObject)
{
	FMinimalViewInfo ViewInfo{};

#if WITH_EDITOR
	if (InWorldContextObject->GetWorld()->WorldType == EWorldType::Editor)
	{
		if (auto ActiveViewport = GEditor->GetActiveViewport())
		{
			if (auto EditorViewClient = static_cast<FEditorViewportClient*>(ActiveViewport->GetClient()))
			{
				FIntPoint ViewportOrigin;
				FIntPoint ViewportSize;
				EditorViewClient->GetViewportDimensions(ViewportOrigin, ViewportSize);

				ViewInfo.AspectRatio = (float)ViewportSize.X / (float)ViewportSize.Y;
				ViewInfo.DesiredFOV = EditorViewClient->FOVAngle;
				ViewInfo.FOV = ViewInfo.DesiredFOV;
				ViewInfo.Location = EditorViewClient->GetViewLocation();
				ViewInfo.ProjectionMode = TEnumAsByte<ECameraProjectionMode::Type>(ECameraProjectionMode::Perspective);
				ViewInfo.Rotation = EditorViewClient->GetViewRotation();
				ViewInfo.bUseFieldOfViewForLOD = false;
				ViewInfo.bConstrainAspectRatio = false;
			}
		}
	}
	else
#endif
	{
		if (const auto& Controller = InWorldContextObject->GetWorld()->GetFirstPlayerController())
		{
			FVector2d ViewportSize;
			InWorldContextObject->GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

			ViewInfo.AspectRatio = (float)ViewportSize.X / (float)ViewportSize.Y;
			ViewInfo.DesiredFOV = Controller->PlayerCameraManager->GetFOVAngle();
			ViewInfo.FOV = ViewInfo.DesiredFOV;
			ViewInfo.Location = Controller->PlayerCameraManager->GetCameraLocation();
			ViewInfo.ProjectionMode = TEnumAsByte<ECameraProjectionMode::Type>(ECameraProjectionMode::Perspective);
			ViewInfo.Rotation = Controller->PlayerCameraManager->GetCameraRotation();
			ViewInfo.bUseFieldOfViewForLOD = false;
			ViewInfo.bConstrainAspectRatio = false;
		}
	}

	FMatrix View;
	FMatrix Projection;
	FMatrix ViewProjection;
	UGameplayStatics::GetViewProjectionMatrix(ViewInfo, View, Projection, ViewProjection);

	return ViewProjection;
}
