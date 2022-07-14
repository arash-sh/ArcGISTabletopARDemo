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

#include "CoreMinimal.h"

#include "ArcGISMapsSDK/Utils/AuthGuid.h"

#include "ArcGISAuthenticationConfigurationInstanceData.generated.h"

UENUM()
enum class EArcGISAuthenticationConfigurationType : uint8
{
	OAuth = 0,
};

USTRUCT(Blueprintable)
struct ARCGISMAPSSDK_API FArcGISAuthenticationConfigurationInstanceData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "ArcGISMapsSDK|AuthenticationConfiguration")
	FAuthGuid ID;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapsSDK|AuthenticationConfiguration")
	FString Name;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapsSDK|AuthenticationConfiguration")
	FString ClientID;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapsSDK|AuthenticationConfiguration", meta = (EditConditionHides, EditCondition = "false"))
	FString ClientSecret;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapsSDK|AuthenticationConfiguration")
	FString RedirectURI;

private:
	EArcGISAuthenticationConfigurationType Type = EArcGISAuthenticationConfigurationType::OAuth;
};
