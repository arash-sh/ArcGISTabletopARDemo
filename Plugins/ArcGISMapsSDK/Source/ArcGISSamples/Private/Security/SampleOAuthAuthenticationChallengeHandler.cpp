// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#include "Security/SampleOAuthAuthenticationChallengeHandler.h"

#include "Internationalization/Regex.h"
#if !PLATFORM_ANDROID && !PLATFORM_IOS
#include "Framework/Application/SlateApplication.h"
#if PLATFORM_MAC
#include "Interfaces/IPluginManager.h"
#include "Misc/MessageDialog.h"
#endif
#include "WebBrowserModule.h"
#include "Widgets/Layout/SBorder.h"
#else
#include "WebAuth.h"
#include "WebAuthModule.h"
#endif

#define LOCTEXT_NAMESPACE "ArcGISMapsSDK"

#if !PLATFORM_ANDROID && !PLATFORM_IOS
void USampleOAuthAuthenticationChallengeHandler::HandleChallengeInWebBrowser(UArcGISOAuthAuthenticationChallenge* InOAuthAuthenticationChallenge)
{
#if PLATFORM_MAC
	auto webBrowserWidgetPlugin = IPluginManager::Get().FindPlugin("WebBrowserWidget");

	if (webBrowserWidgetPlugin.IsValid() && !webBrowserWidgetPlugin->IsEnabled())
	{
		FMessageDialog::Open(EAppMsgType::Ok,
							 LOCTEXT("WebBrowserWidgetPluginNotEnabled",
									 "Web Browser plugin is not enabled. Please enable it in the plugin manager to use OAuth with the ArcGIS Maps SDK."));

		InOAuthAuthenticationChallenge->Respond({});

		return;
	}

	// Call to GetSingleton initializes WebBrowser
	if (!(IWebBrowserModule::Get().IsWebModuleAvailable() && IWebBrowserModule::Get().GetSingleton() != nullptr))
	{
		UE_LOG(LogTemp, Verbose, TEXT("ArcGISMapsSDK: WebBrowserModule is not available"));
	}
#endif

	Challenge = ::MakeShared<FChallenge>();

	Challenge->OAuthAuthenticationChallenge = TStrongObjectPtr<UArcGISOAuthAuthenticationChallenge>(InOAuthAuthenticationChallenge);
	Challenge->Window = SNew(SWindow).ClientSize(FVector2D(900, 700));

	FCreateBrowserWindowSettings WindowSettings;

	WindowSettings.bUseTransparency = true;
	WindowSettings.bShowErrorMessage = false;
	WindowSettings.InitialURL = InOAuthAuthenticationChallenge->GetAuthorizeURI();

	Challenge->WebBrowserWindow = IWebBrowserModule::Get().GetSingleton()->CreateBrowserWindow(WindowSettings);

	Challenge->WebBrowserWindow->OnTitleChanged().AddLambda([this](FString NewTitle) {
		if (NewTitle.StartsWith("SUCCESS code="))
		{
			if (!Challenge->bResponded.Exchange(true))
			{
				FSlateApplicationBase::Get().RequestDestroyWindow(Challenge->Window.ToSharedRef());

				NewTitle.ReplaceInline(TEXT("SUCCESS code="), TEXT(""));

				Challenge->OAuthAuthenticationChallenge->Respond(NewTitle);
			}
		}
	});

	Challenge->Window->GetOnWindowClosedEvent().AddLambda([this](const TSharedRef<SWindow>&) {
		if (!Challenge->bResponded.Exchange(true))
		{
			Challenge->OAuthAuthenticationChallenge->Respond({});
		}
	});

	// Setup browser widget.
	TSharedPtr<SWebBrowser> BrowserWidget;

	// clang-format off
	Challenge->Window->SetContent(
		SNew(SBorder)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.Padding(0)
			[
				SAssignNew(BrowserWidget, SWebBrowser, Challenge->WebBrowserWindow)
					.ShowControls(false)
					.ShowAddressBar(false)
			]
	);
	// clang-format on

	FSlateApplication::Get().AddWindow(Challenge->Window.ToSharedRef(), true);
	Challenge->Window->BringToFront();
	FSlateApplication::Get().SetKeyboardFocus(BrowserWidget, EFocusCause::SetDirectly);
}
#else
void USampleOAuthAuthenticationChallengeHandler::HandleChallengeInWebAuthSession(UArcGISOAuthAuthenticationChallenge* InOAuthAuthenticationChallenge,
																				 const FString& InRedirectURI)
{
	Challenge = ::MakeShared<FChallenge>();

	Challenge->OAuthAuthenticationChallenge = TStrongObjectPtr<UArcGISOAuthAuthenticationChallenge>(InOAuthAuthenticationChallenge);

	auto& webAuth = FWebAuthModule::Get().GetWebAuth();

	auto authorizeURI = InOAuthAuthenticationChallenge->GetAuthorizeURI();

	webAuth.AuthSessionWithURL(authorizeURI, InRedirectURI,
							   FWebAuthSessionCompleteDelegate::CreateLambda([this, InRedirectURI](FString RedirectURL, bool bHasResponse) {
								   auto pattern = InRedirectURI + "?code=";

								   if (RedirectURL.StartsWith(pattern))
								   {
									   if (!Challenge->bResponded.Exchange(true))
									   {
										   RedirectURL.ReplaceInline(*pattern, TEXT(""));

										   Challenge->OAuthAuthenticationChallenge->Respond(RedirectURL);
									   }
								   }
								   else
								   {
									   if (!RedirectURL.IsEmpty())
									   {
										   UE_LOG(LogTemp, Error, TEXT("Invalid redirect URI"));
									   }

									   Challenge->OAuthAuthenticationChallenge->Respond({});
								   }
							   }));
}
#endif

FString USampleOAuthAuthenticationChallengeHandler::GetRedirectURI(const FString& InAuthorizeURI)
{
	FRegexPattern RegexPattern("redirect_uri=([^&]*)");
	FRegexMatcher RegexMatcher(RegexPattern, InAuthorizeURI);

	if (!RegexMatcher.FindNext())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to get the authorize URI"));
		return {};
	}

	auto redirectURI = RegexMatcher.GetCaptureGroup(1);

#if !PLATFORM_ANDROID && !PLATFORM_IOS
	if (redirectURI != "urn:ietf:wg:oauth:2.0:oob")
	{
		UE_LOG(LogTemp, Error, TEXT("\"urn:ietf:wg:oauth:2.0:oob\" is the only supported redirect URI"));
		return {};
	}
#else
	if (redirectURI == "urn:ietf:wg:oauth:2.0:oob")
	{
		UE_LOG(LogTemp, Error, TEXT("\"urn:ietf:wg:oauth:2.0:oob\" is not a supported redirect URI"));
		return {};
	}
#endif

	return redirectURI;
}

TEnumAsByte<EArcGISAuthenticationChallengeType::Type> USampleOAuthAuthenticationChallengeHandler::GetType() const
{
	return EArcGISAuthenticationChallengeType::ArcGISOAuthAuthenticationChallenge;
}

void USampleOAuthAuthenticationChallengeHandler::HandleChallenge(TObjectPtr<UArcGISAuthenticationChallenge> InAuthenticationChallenge)
{
	auto oauthAuthenticationChallenge = Cast<UArcGISOAuthAuthenticationChallenge>(InAuthenticationChallenge);

	auto redirectURI = GetRedirectURI(oauthAuthenticationChallenge->GetAuthorizeURI());

	if (redirectURI.IsEmpty())
	{
		oauthAuthenticationChallenge->Respond({});
		return;
	}

#if !PLATFORM_ANDROID && !PLATFORM_IOS
	HandleChallengeInWebBrowser(oauthAuthenticationChallenge);
#else
	HandleChallengeInWebAuthSession(oauthAuthenticationChallenge, redirectURI);
#endif
}

#undef LOCTEXT_NAMESPACE
