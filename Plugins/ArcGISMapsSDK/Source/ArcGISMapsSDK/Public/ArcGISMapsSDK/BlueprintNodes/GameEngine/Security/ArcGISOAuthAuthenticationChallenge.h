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
#include "UObject/Object.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Security/ArcGISAuthenticationChallenge.h"

#include "ArcGISOAuthAuthenticationChallenge.generated.h"

namespace Esri
{
namespace GameEngine
{
namespace Security
{
class ArcGISOAuthAuthenticationChallenge;
} // namespace Security
} // namespace GameEngine
} // namespace Esri

UCLASS(BlueprintType, Blueprintable)
class ARCGISMAPSSDK_API UArcGISOAuthAuthenticationChallenge :
    public UArcGISAuthenticationChallenge
{
    GENERATED_BODY()

public:
    #pragma region Properties
    /// The current authorization endpoint uri
    /// 
    /// - Since: 100.11.0
    UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISOAuthAuthenticationChallenge")
    FString GetAuthorizeURI();
    #pragma endregion Properties
    
    #pragma region Methods
    /// Respond to the challenge with a token
    /// 
    /// - Since: 100.11.0
    UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISOAuthAuthenticationChallenge")
    void Respond(FString token);
    #pragma endregion Methods
};