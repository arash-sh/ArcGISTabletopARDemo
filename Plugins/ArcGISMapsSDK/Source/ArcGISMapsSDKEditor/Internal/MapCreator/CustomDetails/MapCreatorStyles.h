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
#include "Styling/SlateStyle.h"
#include "Styling/SlateTypes.h"

const FLinearColor ColorFontInner = FLinearColor(0.9f, 0.9f, 0.9f);
const FMargin TextBoxPadding(12, 3);

class MapCreatorStyles
{
public:
	static void Initialize();
	static void Shutdown();

	static const FSlateStyleSet* GetCommonStyleSet();

private:
	static TSharedPtr<FSlateStyleSet> CommonStyleSet;
	static TSharedRef<FSlateStyleSet> CreateCommonStyleSet();

	static void UnregisterStyle(TSharedPtr<FSlateStyleSet> Style);
};
