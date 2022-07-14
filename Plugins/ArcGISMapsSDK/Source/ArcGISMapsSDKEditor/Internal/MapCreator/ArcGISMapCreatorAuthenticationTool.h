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
#include "InteractiveToolBuilder.h"
#include "UObject/NoExportTypes.h"

#include "ArcGISMapsSDK/Security/ArcGISAuthenticationConfigurationInstanceData.h"

#include "ArcGISMapCreatorAuthenticationTool.generated.h"

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorAuthenticationToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	// UInteractiveToolBuilder
	bool CanBuildTool(const FToolBuilderState& SceneState) const override;
	UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
	void PostSetupTool(UInteractiveTool* Tool, const FToolBuilderState& SceneState) const override;
};

UCLASS(Transient)
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorAuthenticationToolProperties : public UInteractiveToolPropertySet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = APIKey, meta = (DisplayName = "API Key"))
	FString APIKey;

	UPROPERTY(EditAnywhere, Category = AddAuthenticationConfiguration, meta = (DisplayName = "Configuration Name"))
	FString Name;

	UPROPERTY(EditAnywhere, Category = AddAuthenticationConfiguration, meta = (DisplayName = "Client ID"))
	FString ClientID;

	UPROPERTY(EditAnywhere, Category = AddAuthenticationConfiguration, meta = (DisplayName = "Redirect URI"))
	FString RedirectURI;

	UPROPERTY(EditAnywhere, Category = AuthenticationConfigurations, meta = (DisplayName = "Configurations"))
	TArray<FArcGISAuthenticationConfigurationInstanceData> Configs;
};

UCLASS()
class ARCGISMAPSSDKEDITOR_API UArcGISMapCreatorAuthenticationTool : public UInteractiveTool
{
	GENERATED_BODY()

public:
	// UInteractiveTool
	void Setup() override;
	void OnPropertyModified(UObject* PropertySet, FProperty* Property);

	UPROPERTY()
	TObjectPtr<UArcGISMapCreatorAuthenticationToolProperties> Properties;
};
