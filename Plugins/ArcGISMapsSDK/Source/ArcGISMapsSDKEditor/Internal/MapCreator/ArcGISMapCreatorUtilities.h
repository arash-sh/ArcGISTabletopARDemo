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
#include "DetailCategoryBuilder.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

namespace MapCreatorUtilities
{
	template<class T>
	T* CheckForComponent(UWorld* World)
	{
		TArray<AActor*> ActorList;
		UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), ActorList);

		uint32 componentCount = 0;
		T* component = nullptr;

		for (auto actor : ActorList)
		{
			auto foundComponent = actor->FindComponentByClass<T>();

			if (foundComponent)
			{
				if (componentCount == 0)
				{
					component = foundComponent;
				}

				++componentCount;
			}
		}
		if (componentCount > 1)
		{
			UE_LOG(LogTemp, Error, TEXT("Multiple components were found"));
		}

		return component;
	}

	class ARCGISMAPSSDKEDITOR_API MapComponentManager
	{
	public:
		static UArcGISMapComponent* GetCurrentMapComponent();
		static void SetCurrentMapComponent(UArcGISMapComponent* mapComponent);

	private:
		static UWorld* CurrentWorld;
		static UArcGISMapComponent* CurrentMapComponent;
	};

	class ARCGISMAPSSDKEDITOR_API MapCreatorWidgetFactory
	{
	public:
		static void CreateFileSelectorField(IDetailCategoryBuilder& category, const FString& categoryName, TSharedRef<IPropertyHandle> inProperty, FString* propertySource, TSharedPtr<FSimpleDelegate> callback);
	private:
		static FString PromptForFile();
	};
}
