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

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Security/ArcGISAuthenticationChallenge.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Security/ArcGISAuthenticationChallengeType.h"

#include "ArcGISAuthenticationChallengeHandler.generated.h"

UCLASS(Abstract)
class ARCGISMAPSSDK_API UArcGISAuthenticationChallengeHandler : public UObject
{
	GENERATED_BODY()

public:
	virtual TEnumAsByte<EArcGISAuthenticationChallengeType::Type> GetType() const
		PURE_VIRTUAL(UArcGISAuthenticationChallengeHandler::GetType, return EArcGISAuthenticationChallengeType::Null;);
	virtual void HandleChallenge(TObjectPtr<UArcGISAuthenticationChallenge> InAuthenticationChallenge)
		PURE_VIRTUAL(UArcGISAuthenticationChallengeHandler::HandleChallenge);
};
