// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#pragma once

#include "UObject/StrongObjectPtr.h"
#if !PLATFORM_ANDROID && !PLATFORM_IOS
#include "IWebBrowserWindow.h"
#include "Widgets/SWindow.h"
#endif

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Security/ArcGISOAuthAuthenticationChallenge.h"
#include "ArcGISMapsSDK/Security/ArcGISAuthenticationChallengeHandler.h"

#include "SampleOAuthAuthenticationChallengeHandler.generated.h"

UCLASS()
class ARCGISSAMPLES_API USampleOAuthAuthenticationChallengeHandler : public UArcGISAuthenticationChallengeHandler
{
	GENERATED_BODY()

public:
	// UArcGISAuthenticationChallengeHandler
	TEnumAsByte<EArcGISAuthenticationChallengeType::Type> GetType() const override;
	void HandleChallenge(TObjectPtr<UArcGISAuthenticationChallenge> InAuthenticationChallenge) override;

private:
	struct FChallenge
	{
		TAtomic<bool> bResponded{false};

		TStrongObjectPtr<UArcGISOAuthAuthenticationChallenge> OAuthAuthenticationChallenge;
#if !PLATFORM_ANDROID && !PLATFORM_IOS
		TSharedPtr<IWebBrowserWindow> WebBrowserWindow;
		TSharedPtr<SWindow> Window;
#endif
	};

#if !PLATFORM_ANDROID && !PLATFORM_IOS
	void HandleChallengeInWebBrowser(UArcGISOAuthAuthenticationChallenge* InOAuthAuthenticationChallenge);
#else
	void HandleChallengeInWebAuthSession(UArcGISOAuthAuthenticationChallenge* InOAuthAuthenticationChallenge, const FString& InRedirectURI);
#endif
	FString GetRedirectURI(const FString& InAuthorizeURI);

	TSharedPtr<FChallenge> Challenge;
};
