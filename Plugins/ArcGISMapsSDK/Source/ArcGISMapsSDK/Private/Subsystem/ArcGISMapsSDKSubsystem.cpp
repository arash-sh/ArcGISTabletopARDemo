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
#include "ArcGISMapsSDK/Subsystem/ArcGISMapsSDKSubsystem.h"

#include "Async/Async.h"
#if WITH_EDITOR
#include "Editor.h"
#endif
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationChallengeInternal.h"
#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationChallengeType.h"
#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationManager.h"
#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISOAuthAuthenticationChallenge.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Security/ArcGISOAuthAuthenticationChallenge.h"

namespace
{
bool IsArcGISMapActorValid(AArcGISMapActor* InMapActor, UWorld* CurrentWorld)
{
	bool isValid = InMapActor && InMapActor->GetWorld() == CurrentWorld;

#if WITH_EDITOR
	isValid = isValid && !InMapActor->bIsEditorPreviewActor && !InMapActor->HasAnyFlags(RF_Transient);
#endif

	return isValid;
}

bool IsGeoReferencingSystemValid(AGeoReferencingSystem* InGeoReferencingSystem, UWorld* CurrentWorld)
{
	bool isValid = InGeoReferencingSystem && InGeoReferencingSystem->GetWorld() == CurrentWorld;

#if WITH_EDITOR
	isValid = isValid && !InGeoReferencingSystem->bIsEditorPreviewActor && !InGeoReferencingSystem->HasAnyFlags(RF_Transient);
#endif

	return isValid;
}
} // namespace

void UArcGISMapsSDKSubsystem::Deinitialize()
{
	GetWorld()->OnLevelsChanged().RemoveAll(this);

#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		GEngine->OnLevelActorAdded().RemoveAll(this);
		GEngine->OnLevelActorDeleted().RemoveAll(this);
		GEngine->OnLevelActorListChanged().RemoveAll(this);
	}
#endif

	Listeners.Empty();

	using namespace Esri::GameEngine::Security;

	ArcGISAuthenticationManager::SetAuthenticationChallengeIssued(PreviousAuthenticationChallengeEventHandler);

	Super::Deinitialize();
}

AGeoReferencingSystem* UArcGISMapsSDKSubsystem::GetGeoReferencingSystem()
{
	return GeoReferencingSystem;
}

UArcGISMapComponent* UArcGISMapsSDKSubsystem::GetMapComponent()
{
	if (MapActor)
	{
		return MapActor->GetMapComponent();
	}

	return nullptr;
}

bool UArcGISMapsSDKSubsystem::HasAuthenticationChallengeHandler()
{
	return AuthenticationChallengeHandler.IsValid();
}

void UArcGISMapsSDKSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if WITH_EDITOR
	if (GetWorld()->WorldType == EWorldType::Editor)
	{
		GEngine->OnLevelActorAdded().AddUObject(this, &UArcGISMapsSDKSubsystem::OnLevelActorAdded);
		GEngine->OnLevelActorDeleted().AddUObject(this, &UArcGISMapsSDKSubsystem::OnLevelActorDeleted);
		GEngine->OnLevelActorListChanged().AddUObject(this, &UArcGISMapsSDKSubsystem::OnLevelActorListChanged);
	}
#endif

	GetWorld()->OnLevelsChanged().AddUObject(this, &UArcGISMapsSDKSubsystem::OnLevelsChanged);

	using namespace Esri::GameEngine::Security;

	PreviousAuthenticationChallengeEventHandler = ArcGISAuthenticationManager::GetAuthenticationChallengeIssued();

	ArcGISAuthenticationManager::SetAuthenticationChallengeIssued([this](ArcGISAuthenticationChallenge& AuthenticationChallenge) {
		auto authenticationChallengeType = ArcGISAuthenticationChallengeInternal::GetObjectType(&AuthenticationChallenge);

		switch (authenticationChallengeType)
		{
			case ArcGISAuthenticationChallengeType::ArcGISOAuthAuthenticationChallenge:
			{
				auto arcGISOAuthAuthenticationChallengeAPIObject = ::MakeShared<Esri::GameEngine::Security::ArcGISOAuthAuthenticationChallenge>(
					std::move(static_cast<Esri::GameEngine::Security::ArcGISOAuthAuthenticationChallenge&>(AuthenticationChallenge)));

				AsyncTask(ENamedThreads::GameThread, [this, arcGISOAuthAuthenticationChallengeAPIObject]() {
					if (AuthenticationChallengeHandler.IsValid())
					{
						auto arcGISOAuthAuthenticationChallenge = NewObject<UArcGISOAuthAuthenticationChallenge>();

						arcGISOAuthAuthenticationChallenge->APIObject = arcGISOAuthAuthenticationChallengeAPIObject;

						AuthenticationChallengeHandler->HandleChallenge(arcGISOAuthAuthenticationChallenge);
					}
				});
			}
			break;
			default:
				UE_LOG(LogTemp, Error, TEXT("Unsupported AuthenticationChallenge type"));
				return;
		}
	});
}

#if WITH_EDITOR
void UArcGISMapsSDKSubsystem::OnLevelActorAdded(AActor* InActor)
{
	if (auto AsArcGISMapActor = Cast<AArcGISMapActor>(InActor))
	{
		if (IsArcGISMapActorValid(AsArcGISMapActor, GetWorld()))
		{
			Refresh();
		}
	}
	else if (auto AsAGeoReferencingSystem = Cast<AGeoReferencingSystem>(InActor))
	{
		if (IsGeoReferencingSystemValid(AsAGeoReferencingSystem, GetWorld()))
		{
			Refresh();
		}
	}
}

void UArcGISMapsSDKSubsystem::OnLevelActorDeleted(AActor* InActor)
{
	OnLevelActorAdded(InActor);
}

void UArcGISMapsSDKSubsystem::OnLevelActorListChanged()
{
	Refresh();
}
#endif

void UArcGISMapsSDKSubsystem::OnLevelsChanged()
{
	Refresh();
}

void UArcGISMapsSDKSubsystem::Refresh()
{
	RefreshArcGISMapActor();
	RefreshGeoReferencingSystem();
}

void UArcGISMapsSDKSubsystem::RefreshArcGISMapActor()
{
	auto PreviousMapActor = MapActor;

	MapActor = nullptr;

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArcGISMapActor::StaticClass(), Actors);

	auto ActorsCount = Actors.Num();

	if (ActorsCount == 1)
	{
		MapActor = Cast<AArcGISMapActor>(Actors[0]);

		if (MapActor != PreviousMapActor)
		{
			for (auto Listener : Listeners)
			{
				Listener->OnArcGISMapComponentChanged(MapActor->GetMapComponent());
			}
		}
	}
	else if (ActorsCount > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Multiple ArcGISMapActor actors found. Only one actor should be present in the world."));
	}
}

void UArcGISMapsSDKSubsystem::RefreshGeoReferencingSystem()
{
	auto PreviousGeoReferencingSystem = GeoReferencingSystem;

	GeoReferencingSystem = nullptr;

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGeoReferencingSystem::StaticClass(), Actors);

	auto ActorsCount = Actors.Num();

	if (ActorsCount == 1)
	{
		GeoReferencingSystem = Cast<AGeoReferencingSystem>(Actors[0]);

		if (PreviousGeoReferencingSystem != GeoReferencingSystem)
		{
			for (auto Listener : Listeners)
			{
				Listener->OnGeoReferencingSystemChanged(GeoReferencingSystem);
			}
		}
	}
	else if (ActorsCount > 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Multiple GeoReferencingSystem actors found. Only one actor should be present in the world."));
	}
}

void UArcGISMapsSDKSubsystem::RegisterListener(IArcGISMapsSDKSubsystemListener* InListener)
{
	if (Listeners.AddUnique(InListener) == Listeners.Num() - 1)
	{
		if (MapActor)
		{
			InListener->OnArcGISMapComponentChanged(MapActor->GetMapComponent());
		}

		if (GeoReferencingSystem)
		{
			InListener->OnGeoReferencingSystemChanged(GeoReferencingSystem);
		}
	}
}

void UArcGISMapsSDKSubsystem::SetAuthenticationChallengeHandler(TObjectPtr<UArcGISAuthenticationChallengeHandler> InAuthenticationChallengeHandler)
{
	if (InAuthenticationChallengeHandler->GetType() != EArcGISAuthenticationChallengeType::ArcGISOAuthAuthenticationChallenge)
	{
		UE_LOG(LogTemp, Error, TEXT("Unsupported AuthenticationChallengeHandler type"));
		return;
	}

	AuthenticationChallengeHandler = TStrongObjectPtr<UArcGISAuthenticationChallengeHandler>(InAuthenticationChallengeHandler);
}

void UArcGISMapsSDKSubsystem::UnregisterListener(IArcGISMapsSDKSubsystemListener* InListener)
{
	Listeners.Remove(InListener);
}
