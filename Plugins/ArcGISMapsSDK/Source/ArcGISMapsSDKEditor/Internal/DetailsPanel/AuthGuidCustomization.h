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
#include "IPropertyTypeCustomization.h"
#include "Widgets/Input/STextComboBox.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

class FAuthGuidCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	// IPropertyTypeCustomization
	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
						 FDetailWidgetRow& HeaderRow,
						 IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
						   IDetailChildrenBuilder& ChildBuilder,
						   IPropertyTypeCustomizationUtils& CustomizationUtils) override;
private:
	void OnAuthenticationChanged();
	void UpdateAuthConfigOptions();
	void ValidateIDs();
	void SetSelection(TSharedPtr<FString> Item, ESelectInfo::Type Type);
	FString GetSelection(TSharedPtr<FString> StringItem);
	TSharedPtr<FString> GetCurrentConfigOption();


	bool bElevationAuth = false;
	bool bBasemapAuth = false;
	UArcGISMapComponent* MapComponent;
	FAuthGuid* GuidData;
	TSharedPtr<IPropertyHandle> ParentPropertyHandle;
	TArray<TSharedPtr<FString>> AuthConfigOptions;
	TSharedPtr<STextComboBox> AuthConfigList;
};
