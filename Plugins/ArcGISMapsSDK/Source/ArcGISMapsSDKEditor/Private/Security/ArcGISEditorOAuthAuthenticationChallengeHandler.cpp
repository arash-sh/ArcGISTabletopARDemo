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
#include "Security/ArcGISEditorOAuthAuthenticationChallengeHandler.h"

#include "Framework/Application/SlateApplication.h"
#include "Internationalization/Regex.h"
#include "WebBrowserModule.h"
#include "Widgets/Layout/SBorder.h"

#if PLATFORM_MAC
#include "Interfaces/IPluginManager.h"
#include "Misc/MessageDialog.h"
#endif

#define LOCTEXT_NAMESPACE "ArcGISMapsSDK"

void UArcGISEditorOAuthAuthenticationChallengeHandler::HandleChallengeInWebBrowser(
	UArcGISOAuthAuthenticationChallenge* InOAuthAuthenticationChallenge)
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

FString UArcGISEditorOAuthAuthenticationChallengeHandler::GetRedirectURI(const FString& InAuthorizeURI)
{
	FRegexPattern RegexPattern("redirect_uri=([^&]*)");
	FRegexMatcher RegexMatcher(RegexPattern, InAuthorizeURI);

	if (!RegexMatcher.FindNext())
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to get the authorize URI"));
		return {};
	}

	auto redirectURI = RegexMatcher.GetCaptureGroup(1);

	if (redirectURI != "urn:ietf:wg:oauth:2.0:oob")
	{
		UE_LOG(LogTemp, Error, TEXT("\"urn:ietf:wg:oauth:2.0:oob\" is the only supported redirect URI"));
		return {};
	}

	return redirectURI;
}

TEnumAsByte<EArcGISAuthenticationChallengeType::Type> UArcGISEditorOAuthAuthenticationChallengeHandler::GetType() const
{
	return EArcGISAuthenticationChallengeType::ArcGISOAuthAuthenticationChallenge;
}

void UArcGISEditorOAuthAuthenticationChallengeHandler::HandleChallenge(TObjectPtr<UArcGISAuthenticationChallenge> InAuthenticationChallenge)
{
	auto oauthAuthenticationChallenge = Cast<UArcGISOAuthAuthenticationChallenge>(InAuthenticationChallenge);

	auto redirectURI = GetRedirectURI(oauthAuthenticationChallenge->GetAuthorizeURI());

	if (redirectURI.IsEmpty())
	{
		oauthAuthenticationChallenge->Respond({});
		return;
	}

	HandleChallengeInWebBrowser(oauthAuthenticationChallenge);
}

#undef LOCTEXT_NAMESPACE
