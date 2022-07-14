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

#include "DetailsPanel/ArcGISMapComponentExtentProperty.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"
#include "IPropertyUtilities.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

TSharedRef<IPropertyTypeCustomization> FMapComponentExtentPropertyCustomization::MakeInstance()
{
	return MakeShared<FMapComponentExtentPropertyCustomization>();
}

void FMapComponentExtentPropertyCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
															   FDetailWidgetRow& HeaderRow,
															   IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	check(CastFieldChecked<FStructProperty>(PropertyHandle->GetProperty())->Struct == FArcGISExtentInstanceData::StaticStruct());
	HeaderRow.NameContent()[PropertyHandle->CreatePropertyNameWidget()];
}

void FMapComponentExtentPropertyCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
																 IDetailChildrenBuilder& ChildBuilder,
																 IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	auto extentCenterProperty = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FArcGISExtentInstanceData, ExtentCenter));
	ChildBuilder.AddProperty(extentCenterProperty.ToSharedRef());

	SetOnShapeChange(PropertyHandle, ChildBuilder, CustomizationUtils);
}

void FMapComponentExtentPropertyCustomization::SetOnShapeChange(TSharedRef<IPropertyHandle> PropertyHandle,
																IDetailChildrenBuilder& ChildBuilder,
																IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	ShapePropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FArcGISExtentInstanceData, ExtentShape));

	auto ShapeDimensionsPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FArcGISExtentInstanceData, ShapeDimensions));
	ShapeXPropertyHandle = ShapeDimensionsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector2D, X));
	ShapeYPropertyHandle = ShapeDimensionsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FVector2D, Y));

	ShapeYPropertyHandle->SetPropertyDisplayName(FText::FromString("Height"));

	ShapePropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([this, &CustomizationUtils]() {
		OnShapeChanged();
		CustomizationUtils.GetPropertyUtilities()->ForceRefresh();
	}));

	ChildBuilder.AddProperty(ShapePropertyHandle.ToSharedRef());
	ChildBuilder.AddProperty(ShapeXPropertyHandle.ToSharedRef());
	ShapeYRow = &ChildBuilder.AddProperty(ShapeYPropertyHandle.ToSharedRef());
	OnShapeChanged();
}

void FMapComponentExtentPropertyCustomization::OnShapeChanged()
{
	uint8 shapeValue;
	ShapePropertyHandle->GetValue(shapeValue);
	EMapExtentShapes shape = static_cast<EMapExtentShapes>(shapeValue);

	switch (shape)
	{
		case EMapExtentShapes::Square:
			ShapeXPropertyHandle->SetPropertyDisplayName(FText::FromString("Width"));
			ShapeYRow->Visibility(EVisibility::Hidden);
			break;
		case EMapExtentShapes::Rectangle:
			ShapeXPropertyHandle->SetPropertyDisplayName(FText::FromString("Width"));
			ShapeYRow->Visibility(EVisibility::Visible);
			break;
		case EMapExtentShapes::Circle:
			ShapeXPropertyHandle->SetPropertyDisplayName(FText::FromString("Radius"));
			ShapeYRow->Visibility(EVisibility::Hidden);
			break;
		default:
			break;
	}
}
