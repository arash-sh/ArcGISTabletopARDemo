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
#pragma once

#include "GeoReferencingSystem.h"
#include "Subsystems/WorldSubsystem.h"

#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationChallengeIssuedEvent.h"
#include "ArcGISMapsSDK/Actors/ArcGISMapActor.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Security/ArcGISAuthenticationChallengeHandler.h"
#include "ArcGISMapsSDK/Subsystem/IArcGISMapsSDKSubsystemListener.h"

#include "ArcGISMapsSDKSubsystem.generated.h"

UCLASS()
class ARCGISMAPSSDK_API UArcGISMapsSDKSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UArcGISMapComponent* GetMapComponent();
	AGeoReferencingSystem* GetGeoReferencingSystem();

	void RegisterListener(IArcGISMapsSDKSubsystemListener* InListener);
	void UnregisterListener(IArcGISMapsSDKSubsystemListener* InListener);

	bool HasAuthenticationChallengeHandler();
	void SetAuthenticationChallengeHandler(TObjectPtr<UArcGISAuthenticationChallengeHandler> InAuthenticationChallengeHandler);

private:
	// USubsystem
	void Deinitialize() override;
	void Initialize(FSubsystemCollectionBase& Collection) override;

	void OnLevelsChanged();

#if WITH_EDITOR
	void OnLevelActorAdded(AActor* InActor);
	void OnLevelActorDeleted(AActor* InActor);
	void OnLevelActorListChanged();
#endif

	void Refresh();
	void RefreshArcGISMapActor();
	void RefreshGeoReferencingSystem();

	TStrongObjectPtr<UArcGISAuthenticationChallengeHandler> AuthenticationChallengeHandler;
	TObjectPtr<AGeoReferencingSystem> GeoReferencingSystem;
	TArray<TObjectPtr<IArcGISMapsSDKSubsystemListener>> Listeners;
	TObjectPtr<AArcGISMapActor> MapActor;
	Esri::GameEngine::Security::ArcGISAuthenticationChallengeIssuedEvent PreviousAuthenticationChallengeEventHandler;
};
