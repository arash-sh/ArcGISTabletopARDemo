// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#pragma once

#include "CoreMinimal.h"

#include "ArcGISMapsSDK/Actors/ArcGISActor.h"

#include "ArcGISViewStateLoggingComponent.h"
#include "SampleAttributeComponent.h"

#include "SampleAPIMapCreator.generated.h"

// The declaration of our Actor class
UCLASS()
class ARCGISSAMPLES_API ASampleAPIMapCreator : public AArcGISActor
{
	GENERATED_BODY()

public:
	ASampleAPIMapCreator();

	UPROPERTY(EditAnywhere, Category = "ArcGISSamples|SampleAPIMapCreator")
	FString APIKey;

	// UObject
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// IArcGISMapsSDKSubsystemListener
	void OnArcGISMapComponentChanged(UArcGISMapComponent* InMapComponent) override;

	void CreateArcGISMap();

private:
	UPROPERTY(Category = "ArcGISSamples|SampleAPIMapCreator", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USample3DAttributesComponent> AttributeComponent;

	UPROPERTY(Category = "ArcGISSamples|SampleAPIMapCreator", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UArcGISViewStateLoggingComponent> ViewStateLogging;
};
