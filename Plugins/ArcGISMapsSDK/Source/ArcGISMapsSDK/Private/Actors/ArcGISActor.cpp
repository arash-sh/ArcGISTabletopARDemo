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
#include "ArcGISMapsSDK/Actors/ArcGISActor.h"

#include "ArcGISMapsSDK/Subsystem/ArcGISMapsSDKSubsystem.h"

void AArcGISActor::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	if (!IsPendingKillPending())
	{
		if (UWorld* OwningWorld = GetWorld(); OwningWorld && OwningWorld->WorldType != EWorldType::Inactive)
		{
			if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
			{
				arcGISMapsSDKSubsystem->RegisterListener(this);
			}
		}
	}
}

void AArcGISActor::UnregisterAllComponents(bool bForReregister)
{
	if (UWorld* OwningWorld = GetWorld(); OwningWorld && OwningWorld->WorldType != EWorldType::Inactive)
	{
		if (auto arcGISMapsSDKSubsystem = OwningWorld->GetSubsystem<UArcGISMapsSDKSubsystem>())
		{
			arcGISMapsSDKSubsystem->UnregisterListener(this);
		}
	}

	Super::UnregisterAllComponents(bForReregister);
}

void AArcGISActor::OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent)
{
	MapComponent = InMapComponent;
}
