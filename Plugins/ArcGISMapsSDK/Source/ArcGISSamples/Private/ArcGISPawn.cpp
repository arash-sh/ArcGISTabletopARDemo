// Copyright 2022 Esri.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at: http://www.apache.org/licenses/LICENSE-2.0
//
#include "ArcGISPawn.h"

// Unreal

#include "Engine/World.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"

// ArcGISMapsSDK

#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"

AArcGISPawn::AArcGISPawn() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	ArcGISCamera = CreateDefaultSubobject<UArcGISCameraComponent>(TEXT("ArcGISCamera"));
	ArcGISLocation = CreateDefaultSubobject<UArcGISLocationComponent>(TEXT("ArcGISLocation"));

	ArcGISCamera->SetupAttachment(this->GetRootComponent());
	ArcGISLocation->SetupAttachment(this->GetRootComponent());

#if WITH_EDITOR
	ArcGISCamera->bVisualizeComponent = true;
#endif

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

FVector3d AArcGISPawn::GetUpVector()
{
	auto MapComponent = UArcGISMapComponent::GetMapComponent(this);
	if (MapComponent)
	{
		auto PawnENUToViewENU = MapComponent->GetENUAtLocationToViewENUTransform(GetActorLocation());
		return PawnENUToViewENU.GetUnitAxis(EAxis::Z);
	}
	return FVector3d(0, 0, 1);
}

void AArcGISPawn::AddControllerPitchInput(float Value)
{
	if (Controller && Value != 0.f)
	{
		auto controller = (APlayerController*)Controller;

		if (controller->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			auto cameraRightVector = GetActorRotation().RotateVector(FVector3d::RightVector);

			// The forward vector is tangent to the earth, so the camera moves parallel to the Earth surface
			// in the direction of the camera. The effect is similar to rotating the earth.
			auto forwardVector = (cameraRightVector ^ GetUpVector());

			AddMovementInput(forwardVector.GetSafeNormal(), Value * PI * GetCameraSpeed());
		}
		else if (controller->IsInputKeyDown(EKeys::RightMouseButton))
		{
			TurnCameraV(Value);
		}
	}
}

void AArcGISPawn::AddControllerYawInput(float Value)
{
	if (Controller && Value != 0.f)
	{
		auto controller = (APlayerController*)Controller;

		if (controller->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			auto cameraRightVector = GetActorRotation().RotateVector(FVector3d::RightVector);

			auto upVector = GetUpVector();

			// The right vector is tangent to the earth, so the camera moves parallel to the Earth surface
			// perpendicular to the direction of the camera. The effect is similar to rotating the earth.
			auto rightVector = (cameraRightVector ^ upVector) ^ upVector;

			AddMovementInput(rightVector.GetSafeNormal(), Value * PI * GetCameraSpeed());
		}
		else if (controller->IsInputKeyDown(EKeys::RightMouseButton))
		{
			TurnCameraH(Value);
		}
	}
}

void AArcGISPawn::TurnAtRate(float Value)
{
	if (Controller && Value != 0.f)
	{
		TurnCameraH(Value);
	}
}

void AArcGISPawn::LookUpAtRate(float Value)
{
	if (Controller && Value != 0.f)
	{
		TurnCameraV(Value);
	}
}

void AArcGISPawn::TurnCameraH(float Value)
{
	auto controller = (APlayerController*)Controller;

	auto InputYawScale =  GetDefault<UInputSettings>()->bEnableLegacyInputScales ? controller->InputYawScale_DEPRECATED : 1.0f;

	FQuat axisRot(GetUpVector(), Value * FMath::DegreesToRadians(InputYawScale));
	SetActorRotation((axisRot * GetActorRotation().Quaternion()).Rotator());
}

void AArcGISPawn::TurnCameraV(float Value)
{
	auto controller = (APlayerController*)Controller;

	auto InputPitchScale =  GetDefault<UInputSettings>()->bEnableLegacyInputScales ? controller->InputPitchScale_DEPRECATED : 1.0f;

	FQuat axisRot(FVector3d::RightVector, -Value * FMath::DegreesToRadians(InputPitchScale));
	SetActorRotation((GetActorRotation().Quaternion() * axisRot).Rotator());
}

double AArcGISPawn::GetCameraSpeed()
{
	UFloatingPawnMovement* movement = (UFloatingPawnMovement*)MovementComponent;
	return movement->MaxSpeed / 100.0;
}

void AArcGISPawn::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X), Value * GetCameraSpeed());
	}
}

void AArcGISPawn::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Y), Value * GetCameraSpeed());
	}
}

void AArcGISPawn::MoveUp_World(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::Z), Value * GetCameraSpeed());
	}
}

void AArcGISPawn::Tick(float DeltaTime)
{
	ApplyEarthCurvatureToMovement();

	Super::Tick(DeltaTime);
}

void AArcGISPawn::ApplyEarthCurvatureToMovement()
{
	auto MapComponent = UArcGISMapComponent::GetMapComponent(this);
	if (MapComponent && MapComponent->HasSpatialReference())
	{
		auto PawnENUToViewENU = MapComponent->GetENUAtLocationToViewENUTransform(GetActorLocation());

		if (!bInitLastCoordinateSystem)
		{
			LastPawnENUToViewENU = PawnENUToViewENU;
			bInitLastCoordinateSystem = true;
		}

		// Adjust rotation to follow the Earth, while preventing spinning at the poles
		FQuat NewRotation = GetActorRotation().Quaternion();
		auto movement = Cast<UFloatingPawnMovement>(GetMovementComponent());

		if (FVector3d::DotProduct(PawnENUToViewENU.GetRotation().Vector(), LastPawnENUToViewENU.GetRotation().Vector()) > 0.8)
		{
			auto DeltaRotation = PawnENUToViewENU.GetRotation() * LastPawnENUToViewENU.GetRotation().Inverse();
			NewRotation = DeltaRotation * NewRotation;
			movement->Velocity = DeltaRotation * movement->Velocity;
		}

		LastPawnENUToViewENU = PawnENUToViewENU;

		SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);

		auto GeographicCoordinates = MapComponent->EngineToGeographic(GetActorLocation());

		if (GeographicCoordinates.IsValid())
		{
			auto GroundDistance = GeographicCoordinates.Z * 100.0;

			movement->MaxSpeed = std::max(PawnSpeedFactor * GroundDistance, 100.);
			movement->Acceleration = movement->MaxSpeed * 3.0;
			movement->Deceleration = movement->Acceleration * 2.0;
		}
	}
}

void AArcGISPawn::SetPawnSpeedFactor(float InPawnSpeedFactor)
{
	PawnSpeedFactor = InPawnSpeedFactor;
}

float AArcGISPawn::GetPawnSpeedFactor() const
{
	return PawnSpeedFactor;
}

