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
#include "DetailsPanel/AuthGuidCustomization.h"

#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"
#include "IPropertyTypeCustomization.h"
#include "IPropertyUtilities.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "MapCreator/ArcGISMapCreatorUtilities.h"

using namespace MapCreatorUtilities;

TSharedRef<IPropertyTypeCustomization> FAuthGuidCustomization::MakeInstance()
{
	return MakeShared<FAuthGuidCustomization>();
}

void FAuthGuidCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle,
													  FDetailWidgetRow& HeaderRow,
													  IPropertyTypeCustomizationUtils& CustomizationUtils)
{

}

void FAuthGuidCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle,
														IDetailChildrenBuilder& ChildBuilder,
														IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	const FText Label = FText::FromString("Authentication");

	MapComponent = MapComponentManager::GetCurrentMapComponent();

	if (!MapComponent)
	{
		return;
	}

	void* guidData;
	check(PropertyHandle->GetValueData(guidData) == FPropertyAccess::Success);
	GuidData = reinterpret_cast<FAuthGuid*>(guidData);

	bElevationAuth = PropertyHandle->GetBoolMetaData(TEXT("ElevationAuth"));
	bBasemapAuth = PropertyHandle->GetBoolMetaData(TEXT("BasemapAuth"));
	ParentPropertyHandle = PropertyHandle->GetParentHandle();

	MapComponent->OnAuthenticationChanged.AddSP(this, &FAuthGuidCustomization::OnAuthenticationChanged);

	ValidateIDs();
	UpdateAuthConfigOptions();

	auto authHeader = PropertyHandle->CreatePropertyNameWidget(Label);
	authHeader->SetToolTipText(Label);

	ChildBuilder.AddCustomRow(Label)
		.NameContent()
		[
			authHeader
		]
		.ValueContent()
		[
			SAssignNew(AuthConfigList, STextComboBox)
			.OptionsSource(&AuthConfigOptions)
			.InitiallySelectedItem(GetCurrentConfigOption())
			.Font(CustomizationUtils.GetRegularFont())
			.OnSelectionChanged(this, &FAuthGuidCustomization::SetSelection)
			.OnGetTextLabelForItem(this, &FAuthGuidCustomization::GetSelection)
		];
}

FString FAuthGuidCustomization::GetSelection(TSharedPtr<FString> StringItem)
{
	return  *StringItem;
}

void FAuthGuidCustomization::OnAuthenticationChanged()
{
	UpdateAuthConfigOptions();
	ValidateIDs();
	AuthConfigList->SetSelectedItem(GetCurrentConfigOption());
}

void FAuthGuidCustomization::UpdateAuthConfigOptions()
{
	AuthConfigOptions.Empty();
	AuthConfigOptions.Add(MakeShareable(new FString("None")));

	for (auto& config : MapComponent->GetAuthenticationConfigurations())
	{
		AuthConfigOptions.Add(MakeShareable(new FString(config.Name)));
	}
}

void FAuthGuidCustomization::ValidateIDs()
{
	for (const auto& config : MapComponent->GetAuthenticationConfigurations())
	{
		if (*GuidData == config.ID)
		{
			return;
		}
	}

	GuidData->Invalidate();
	MapComponent->MarkPackageDirty();
}

TSharedPtr<FString> FAuthGuidCustomization::GetCurrentConfigOption()
{
	const auto& configs = MapComponent->GetAuthenticationConfigurations();

	for (int32 index = 0; index < configs.Num(); index++)
	{
		if (*GuidData == configs[index].ID && *AuthConfigOptions[index + 1].Get() == configs[index].Name)
		{
			return AuthConfigOptions[index + 1];
		}
	}

	return AuthConfigOptions[0];
}

void FAuthGuidCustomization::SetSelection(TSharedPtr<FString> Item, ESelectInfo::Type Type)
{
	if (Item == AuthConfigOptions[0])
	{
		GuidData->Invalidate();
		MapComponent->MarkPackageDirty();
	}
	else
	{
		for (int32 index = 1; index < AuthConfigOptions.Num(); index++)
		{
			if (Item == AuthConfigOptions[index])
			{
				auto configs = MapComponent->GetAuthenticationConfigurations();

				*GuidData = configs[index - 1].ID;
				MapComponent->MarkPackageDirty();
				break;
			}
		}
	}

	if (bElevationAuth)
	{
		MapComponent->SetElevationAuthID(*GuidData);
		return;
	}
	else if (bBasemapAuth)
	{
		MapComponent->SetBasemapAuthID(*GuidData);
		return;
	}

	void* parentData;
	check(ParentPropertyHandle->GetValueData(parentData) == FPropertyAccess::Success);
	auto ParentObject = reinterpret_cast<FArcGISLayerInstanceData*>(parentData);
	if (ParentObject)
	{
		MapComponent->UpdateLayerGuid(ParentObject);
		return;
	}
}
