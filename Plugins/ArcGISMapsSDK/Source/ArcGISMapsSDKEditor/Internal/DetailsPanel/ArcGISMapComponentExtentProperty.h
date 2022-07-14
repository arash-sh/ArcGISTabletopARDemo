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

#include "IDetailPropertyRow.h"
#include "IPropertyTypeCustomization.h"
#include "PropertyHandle.h"

class FMapComponentExtentPropertyCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
								 FDetailWidgetRow& HeaderRow,
								 IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
								   IDetailChildrenBuilder& ChildBuilder,
								   IPropertyTypeCustomizationUtils& CustomizationUtils) override;

private:
	void OnShapeChanged();
	void SetOnShapeChange(TSharedRef<IPropertyHandle> PropertyHandle,
						  IDetailChildrenBuilder& ChildBuilder,
						  IPropertyTypeCustomizationUtils& CustomizationUtils);

	TSharedPtr<IPropertyHandle> ShapePropertyHandle;
	TSharedPtr<IPropertyHandle> ShapeXPropertyHandle;
	TSharedPtr<IPropertyHandle> ShapeYPropertyHandle;
	IDetailPropertyRow* ShapeYRow;
};
