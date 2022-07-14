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

#include "UObject/StrongObjectPtr.h"
#include "IWebBrowserWindow.h"
#include "Widgets/SWindow.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Security/ArcGISOAuthAuthenticationChallenge.h"
#include "ArcGISMapsSDK/Security/ArcGISAuthenticationChallengeHandler.h"

#include "ArcGISEditorOAuthAuthenticationChallengeHandler.generated.h"

UCLASS()
class UArcGISEditorOAuthAuthenticationChallengeHandler : public UArcGISAuthenticationChallengeHandler
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
		TSharedPtr<IWebBrowserWindow> WebBrowserWindow;
		TSharedPtr<SWindow> Window;
	};

	FString GetRedirectURI(const FString& InAuthorizeURI);
	void HandleChallengeInWebBrowser(UArcGISOAuthAuthenticationChallenge* InOAuthAuthenticationChallenge);

	TSharedPtr<FChallenge> Challenge;
};
