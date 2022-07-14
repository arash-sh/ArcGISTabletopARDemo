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

#include "DetailsPanel/ArcGISGeoPositionProperty.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"
#include "IPropertyUtilities.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoPosition.h"

TSharedRef<IPropertyTypeCustomization> FGeoPositionDetailCustomization::MakeInstance()
{
	return MakeShared<FGeoPositionDetailCustomization>();
}

void FGeoPositionDetailCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
													  FDetailWidgetRow& HeaderRow,
													  IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	check(CastFieldChecked<FStructProperty>(PropertyHandle->GetProperty())->Struct == FGeoPosition::StaticStruct());

	HeaderRow.NameContent()
	[
		PropertyHandle->CreatePropertyNameWidget()
	];
}

void FGeoPositionDetailCustomization::OnSRChanged()
{
	int WKID;
	SRPropertyHandle->GetValue(WKID);

	// Reset value to 4326 if a bad value was input.
	// See GeoPosition::GetWKID() which also does the same thing
	try
	{
		UArcGISSpatialReference::CreateArcGISSpatialReference(WKID);
	}
	catch (...)
	{
		WKID = 4326;
		SRPropertyHandle->SetValue(WKID);
	}

	if (WKID == 4326 || WKID == 4490)
	{
		XPropertyHandle->SetPropertyDisplayName(FText::FromString("Longitude"));
		YPropertyHandle->SetPropertyDisplayName(FText::FromString("Latitude"));
		ZPropertyHandle->SetPropertyDisplayName(FText::FromString("Altitude"));
	}
	else
	{
		XPropertyHandle->SetPropertyDisplayName(FText::FromString("X"));
		YPropertyHandle->SetPropertyDisplayName(FText::FromString("Y"));
		ZPropertyHandle->SetPropertyDisplayName(FText::FromString("Z"));
	}
}

void FGeoPositionDetailCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
														IDetailChildrenBuilder& ChildBuilder,
														IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	XPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGeoPosition, X));
	YPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGeoPosition, Y));
	ZPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGeoPosition, Z));
	SRPropertyHandle = PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FGeoPosition, SpatialReferenceWkid));

	SRPropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([this, &CustomizationUtils]() {
		OnSRChanged();

		CustomizationUtils.GetPropertyUtilities()->ForceRefresh();
	}));

	ChildBuilder.AddProperty(XPropertyHandle.ToSharedRef());
	ChildBuilder.AddProperty(YPropertyHandle.ToSharedRef());

	if (!PropertyHandle->GetBoolMetaData(TEXT("HideZ")))
	{
		ChildBuilder.AddProperty(ZPropertyHandle.ToSharedRef());
	}

	ChildBuilder.AddProperty(SRPropertyHandle.ToSharedRef());

	// Initial setup for default value
	OnSRChanged();
}
