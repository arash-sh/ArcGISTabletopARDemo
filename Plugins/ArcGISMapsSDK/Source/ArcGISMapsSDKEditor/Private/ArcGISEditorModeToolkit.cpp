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

#include "ArcGISEditorModeToolkit.h"

// Unreal

#include "EditorModeManager.h"
#include "Engine/Selection.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

// ArcGISMapsSDK

#include "ArcGISEditorMode.h"

#define LOCTEXT_NAMESPACE "ArcGISEditorModeToolkit"

void FArcGISEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FArcGISEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}

FName FArcGISEditorModeToolkit::GetToolkitFName() const
{
	return FName("ArcGISEditorMode");
}

FText FArcGISEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "ArcGISEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
