// COPYRIGHT 1995-2021 ESRI
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

#include "IPropertyTypeCustomization.h"

class FGeoPositionDetailCustomization : public IPropertyTypeCustomization
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

	void OnSRChanged();

private:
	TSharedPtr<IPropertyHandle> XPropertyHandle;
	TSharedPtr<IPropertyHandle> YPropertyHandle;
	TSharedPtr<IPropertyHandle> ZPropertyHandle;
	TSharedPtr<IPropertyHandle> SRPropertyHandle;
};
