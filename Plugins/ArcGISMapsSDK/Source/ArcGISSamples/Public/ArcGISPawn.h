// Copyright 2021 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"

#include "ArcGISMapsSDK/Components/ArcGISCameraComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISLocationComponent.h"

#include "ArcGISPawn.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ARCGISSAMPLES_API AArcGISPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	AArcGISPawn();

	UPROPERTY(VisibleAnywhere, Category = "ArcGISMapsSDK|SampleDefaultPawn")
	UArcGISCameraComponent* ArcGISCamera;

	UPROPERTY(VisibleAnywhere, Category = "ArcGISMapsSDK|SampleDefaultPawn")
	UArcGISLocationComponent* ArcGISLocation;

	void SetPawnSpeedFactor(float InPawnSpeedFactor);
	float GetPawnSpeedFactor() const;

protected:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "ArcGISMapsSDK|SampleDefaultPawn", meta = (ClampMin = "0"))
	float PawnSpeedFactor = 1.0f;

	void MoveUp_World(float Value) override;
	void MoveForward(float Value) override;
	void MoveRight(float Value) override;
	void AddControllerYawInput(float Value) override;
	void AddControllerPitchInput(float Value) override;
	void TurnAtRate(float Value) override;
	void LookUpAtRate(float Value) override;

	double GetCameraSpeed();
	FVector3d GetUpVector();
	void TurnCameraH(float Value);
	void TurnCameraV(float Value);

	void ApplyEarthCurvatureToMovement();

	FTransform LastPawnENUToViewENU;
	bool bInitLastCoordinateSystem = false;
};
