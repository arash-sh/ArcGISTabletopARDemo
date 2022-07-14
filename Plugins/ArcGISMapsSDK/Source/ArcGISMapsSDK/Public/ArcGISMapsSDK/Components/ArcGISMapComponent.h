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
#include "Tasks/Task.h"
#include "UObject/StrongObjectPtr.h"

#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayerType.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMap.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Map/ArcGISMapType.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/View/ArcGISView.h"
#include "ArcGISMapsSDK/Components/ArcGISActorComponent.h"
#include "ArcGISMapsSDK/Memory/IArcGISMemorySystem.h"
#include "ArcGISMapsSDK/Memory/IArcGISMemorySystemHandler.h"
#include "ArcGISMapsSDK/Security/ArcGISAuthenticationConfigurationInstanceData.h"
#include "ArcGISMapsSDK/Utils/AuthGuid.h"
#include "ArcGISMapsSDK/Utils/GeoCoord/GeoPosition.h"
#include "ArcGISMapsSDK/Utils/Unreal/RaycastHelpers.h"
#include "ArcGISMapsSDK/Utils/ViewportProperties.h"

#include "ArcGISMapComponent.generated.h"

UENUM()
enum class EMapExtentShapes : uint8
{
	Square = 0,
	Rectangle = 1,
	Circle = 2
};

UENUM()
enum class EBaseMapTypes : uint8
{
	ImageLayer = 0,
	Basemap = 1,
};

USTRUCT(Blueprintable)
struct FArcGISExtentInstanceData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "MapExtent", meta = (DisplayName = "Geographic Center", HideZ = true))
	FGeoPosition ExtentCenter;

	UPROPERTY(EditAnywhere, Category = "MapExtent", meta = (DisplayName = "Extent Shape"))
	EMapExtentShapes ExtentShape = EMapExtentShapes::Square;

	UPROPERTY(EditAnywhere, Category = "MapExtent", meta = (DisplayName = "Shape Dimensions"))
	FVector2D ShapeDimensions = FVector2D(0, 0);

	bool operator==(const FArcGISExtentInstanceData& other) const
	{
		return ExtentCenter == other.ExtentCenter && ExtentShape == other.ExtentShape && ShapeDimensions == other.ShapeDimensions;
	}

	bool operator!=(const FArcGISExtentInstanceData& other) const
	{
		return ExtentCenter != other.ExtentCenter || ExtentShape != other.ExtentShape || ShapeDimensions != other.ShapeDimensions;
	}
};

USTRUCT(Blueprintable)
struct FArcGISLayerInstanceData
{
	GENERATED_USTRUCT_BODY()

	FGuid ID;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent")
	FString Name;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent")
	FString Source;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	double Opacity = 1;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent")
	bool bIsVisible = true;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent")
	TEnumAsByte<EArcGISLayerType::Type> Type = EArcGISLayerType::ArcGISImageLayer;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent", meta = (DisplayName = "Authentication Configuration ID"))
	FAuthGuid AuthConfigID;

	TStrongObjectPtr<UArcGISLayer> APIObject;

	friend bool operator==(const FArcGISLayerInstanceData& Left, const FArcGISLayerInstanceData& Right);
	friend bool operator!=(const FArcGISLayerInstanceData& Left, const FArcGISLayerInstanceData& Right);
};

UCLASS(ClassGroup = (ArcGISMapsSDK),
	   meta = (BlueprintSpawnableComponent),
	   Category = "ArcGISMapsSDK|ArcGISMapComponent",
	   hidecategories = (Activation, AssetUserData, Collision, Cooking, LOD, Object, Physics, Rendering, SceneComponent, Tags))
class ARCGISMAPSSDK_API UArcGISMapComponent : public UArcGISActorComponent, public IArcGISMemorySystem
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE(FAuthenticationChangedDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEditorModeChangedDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMapTypeChangedDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMeshCollidersEnabledChangedDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOriginPositionChangedDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpatialReferenceChangedDelegate);

public:
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	FString GetAPIkey() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetAPIkey(const FString& APIkey);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	bool GetAreMeshCollidersEnabled() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetAreMeshCollidersEnabled(bool bAreMeshCollidersEnabled);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	const TArray<FArcGISAuthenticationConfigurationInstanceData>& GetAuthenticationConfigurations() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetAuthenticationConfigurations(TArray<FArcGISAuthenticationConfigurationInstanceData> AuthenticationConfigurations);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Basemap")
	FAuthGuid GetBasemapAuthID() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Basemap")
	void SetBasemapAuthID(FAuthGuid bBasemapAuthID);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Basemap")
	const FString& GetBasemapSource() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Basemap")
	void SetBasemapSource(const FString& BasemapSource);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Basemap")
	void SetBasemapSourceAndType(const FString& BasemapSource, EBaseMapTypes BasemapType);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Basemap")
	EBaseMapTypes GetBasemapType() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Basemap")
	void SetBasemapType(EBaseMapTypes BasemapType);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Elevation")
	FString GetElevationSource() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Elevation")
	void SetElevationSource(const FString& ElevationSource);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Elevation")
	FAuthGuid GetElevationAuthID() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Elevation")
	void SetElevationAuthID(FAuthGuid ElevationAuthID);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	const FArcGISExtentInstanceData& GetExtent() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetExtent(const FArcGISExtentInstanceData& Extent);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	bool GetIsEditorModeEnabled() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetIsEditorModeEnabled(bool bIsEditorModeEnabled);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Elevation")
	bool GetIsElevationEnabled() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent|Elevation")
	void SetIsElevationEnabled(bool bIsElevationEnabled);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	bool GetIsExtentEnabled() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetIsExtentEnabled(bool bIsExtentEnabled);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	bool GetIsOriginAtPlanetCenter() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetIsOriginAtPlanetCenter(bool bIsOriginAtPlanetCenter);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	const TArray<FArcGISLayerInstanceData>& GetLayers() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetLayers(TArray<FArcGISLayerInstanceData> Layers);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "ArcGISMapsSDK|ArcGISMapComponent")
	static UArcGISMapComponent* GetMapComponent(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	UArcGISMap* GetMap();
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetMap(UArcGISMap* Map);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	TEnumAsByte<EArcGISMapType::Type> GetMapType() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetMapType(TEnumAsByte<EArcGISMapType::Type> MapType);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	FGeoPosition GetOriginPosition() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetOriginPosition(const FGeoPosition& OriginPosition);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	bool GetShouldUpdateGeoReferencingSystem() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetShouldUpdateGeoReferencingSystem(bool bShouldUpdateGeoReferencingSystem);

	FViewportProperties GetViewportProperties() const;
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	void SetViewportProperties(int32 ViewportWidthPixels,
							   int32 ViewportHeightPixels,
							   float HorizontalFieldOfViewDegrees,
							   float VerticalFieldOfViewDegrees,
							   float VerticalDistortionFactor);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	UArcGISView* GetView();

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|Transformations")
	FGeoPosition EngineToGeographic(const FVector& EngineCoordinates);
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|Transformations")
	FVector GeographicToEngine(const FGeoPosition& GeographicCoordinates);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|Transformations")
	FVector FromEnginePosition(const FVector& EnginePosition);
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|Transformations")
	FVector ToEnginePosition(const FVector& WorldPosition);

	// Returns a transform that allows to go from the ENU space (in UE axes) based at the provided
	// location to the ENU space based at the Origin Location (in UE axes)
	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ENU Transforms")
	FTransform GetENUAtLocationToViewENUTransform(const FVector& EngineCoordinates);

	// Returns a transform that allows to go from the ENU frame (also in UE axes) based at the Origin to the World frame (in UE axes)
	// The Origin is the position set in the ArcGISMapComponent at editor-time that doesn't change at run-time
	FTransform GetENUToWorldTransformAtOrigin();

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	bool ZoomToExtent(AActor* Actor, UArcGISExtent* Extent);

	UE::Tasks::TTask<bool> ZoomToLayerAsync(AActor* InActor, UArcGISLayer* InLayer);

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	bool ZoomToLayer(AActor* Actor, FArcGISLayerInstanceData Layer);

	FAuthenticationChangedDelegate OnAuthenticationChanged;

	FEditorModeChangedDelegate OnEditorModeChanged;

	UPROPERTY(BlueprintAssignable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	FMapTypeChangedDelegate OnMapTypeChanged;

	FMeshCollidersEnabledChangedDelegate OnMeshCollidersEnabledChanged;

	FOriginPositionChangedDelegate OnOriginPositionChanged;

	UPROPERTY(BlueprintAssignable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	FSpatialReferenceChangedDelegate OnSpatialReferenceChanged;

	UFUNCTION(BlueprintCallable, Category = "ArcGISMapsSDK|ArcGISMapComponent")
	FArcGISRaycastHit GetArcGISRaycastHit(FHitResult InRaycastHit);

	IArcGISMemorySystemHandler* GetMemorySystemHandler();
	void SetMemorySystemHandler(IArcGISMemorySystemHandler* InMemorySystemHandler);

	bool HasSpatialReference();

	bool ShouldEditorComponentBeUpdated() const;

	// UObject
	void BeginDestroy() override;
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// UActorComponent
	void OnComponentCreated() override;
	void OnRegister() override;

	// IArcGISMapsSDKSubsystemListener
	void OnGeoReferencingSystemChanged(AGeoReferencingSystem* InGeoReferencingSystem) override;

	//	IArcGISMemorySystem
	virtual void NotifyLowMemoryWarning() override;
	virtual void SetMemoryQuotas(FArcGISMemoryQuotas InMemoryQuotas) override;

	void UpdateLayerGuid(FArcGISLayerInstanceData* InUpdatedLayer);

protected:
	friend class UArcGISMapCreatorBasemapTool;
	friend class UArcGISMapCreatorBasemapToolBuilder;
	friend class UArcGISMapCreatorElevationTool;
	friend class UArcGISMapCreatorElevationToolBuilder;

	// These members are used to store custom values in the case where the default one is active but we still want to be able to persist these in the modes panel
	FString CustomBasemapSource;
	EBaseMapTypes CustomBasemapType;
	FString CustomElevationSource;

private:
	friend class MapCreatorLayerToolDetails;
	friend class UArcGISMapCreatorLayerTool;
	friend class UArcGISMapCreatorLayerToolBuilder;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetAreMeshCollidersEnabled,
			  BlueprintSetter = SetAreMeshCollidersEnabled,
			  Category = "ArcGISMapComponent|Settings",
			  meta = (DisplayName = "Enable Mesh Colliders"))
	bool bAreMeshCollidersEnabled = false;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetIsEditorModeEnabled,
			  BlueprintSetter = SetIsEditorModeEnabled,
			  Category = "ArcGISMapComponent|Settings",
			  meta = (DisplayName = "Enable Editor Mode"))
	bool bIsEditorModeEnabled = true;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetIsOriginAtPlanetCenter,
			  BlueprintSetter = SetIsOriginAtPlanetCenter,
			  Category = "ArcGISMapComponent",
			  meta = (DisplayName = "Origin At Planet Center", EditConditionHides, EditCondition = "MapType==EArcGISMapType::Global"))
	bool bIsOriginAtPlanetCenter = false;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetShouldUpdateGeoReferencingSystem,
			  BlueprintSetter = SetShouldUpdateGeoReferencingSystem,
			  Category = "ArcGISMapComponent",
			  meta = (EditConditionHides, EditCondition = "GeoReferencingSystem!=nullptr"))
	bool bUpdateGeoReferencingSystem = false;

	UPROPERTY()
	AGeoReferencingSystem* GeoReferencingSystem;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetOriginPosition,
			  BlueprintSetter = SetOriginPosition,
			  Category = "ArcGISMapComponent",
			  meta = (EditConditionHides, EditCondition = "bIsOriginAtPlanetCenter==false||MapType==EArcGISMapType::Local"))
	FGeoPosition OriginPosition;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetMapType, BlueprintSetter = SetMapType, Category = "ArcGISMapComponent")
	TEnumAsByte<EArcGISMapType::Type> MapType;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent|Basemap")
	FString Basemap;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent|Basemap")
	EBaseMapTypes BasemapType;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetBasemapAuthID, BlueprintSetter = SetBasemapAuthID, Category = "ArcGISMapComponent|Basemap", meta = (BasemapAuth = true))
	FAuthGuid BasemapAuthConfigID;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetIsElevationEnabled, BlueprintSetter = SetIsElevationEnabled, Category = "ArcGISMapComponent|Elevation")
	bool EnableElevation = true;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetElevationSource,
			  BlueprintSetter = SetElevationSource,
			  Category = "ArcGISMapComponent|Elevation",
			  meta = (EditConditionHides, EditCondition = "EnableElevation==true"))
	FString ElevationSource;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetElevationAuthID,
			  BlueprintSetter = SetElevationAuthID,
			  Category = "ArcGISMapComponent|Elevation",
			  meta = (ElevationAuth = true))
	FAuthGuid ElevationAuthConfigID;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent", meta = (EditConditionHides, EditCondition = "MapType==EArcGISMapType::Local"))
	bool EnableExtent = false;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetExtent,
			  BlueprintSetter = SetExtent,
			  Category = "ArcGISMapComponent",
			  meta = (EditConditionHides, EditCondition = "EnableExtent==true&&MapType==EArcGISMapType::Local"))
	FArcGISExtentInstanceData Extent;

	UPROPERTY(EditAnywhere, BlueprintGetter = GetLayers, BlueprintSetter = SetLayers, Category = "ArcGISMapComponent")
	TArray<FArcGISLayerInstanceData> Layers;

	UPROPERTY(EditAnywhere, Category = "ArcGISMapComponent|Authentication", meta = (DisplayName = "API Key"))
	FString APIKey;

	UPROPERTY(EditAnywhere,
			  BlueprintGetter = GetAuthenticationConfigurations,
			  BlueprintSetter = SetAuthenticationConfigurations,
			  Category = "ArcGISMapComponent|Authentication")
	TArray<FArcGISAuthenticationConfigurationInstanceData> AuthenticationConfigurations;

	TArray<FArcGISLayerInstanceData> LastLayers;
	TStrongObjectPtr<UArcGISMap> Map;
	IArcGISMemorySystemHandler* MemorySystemHandler{nullptr};
	TStrongObjectPtr<UArcGISView> View;

	// Transforms a point from the ENU frame to the World frame
	FMatrix44d ENUFrameToWorldFrame;
	// Transforms a point from the World frame to the ENU frame
	FMatrix44d WorldFrameToENUFrame;
	// Transforms a point from meters to centimeters and applies an axis conversion to UE's axis convention
	FMatrix44d ToUnrealCoordSystem;
	// Transforms a point from centimeters to meters and applies an axis conversion from UE's axis convention
	FMatrix44d FromUnrealCoordSystem;

	// Used by terrain occlusion
	FViewportProperties CurrentViewportProperties;

	void ApplyAPIKeyUpdate();
	void ApplyBasemapUpdate();
	void ApplyEditorModeUpdate();
	void ApplyElevationUpdate();
	void ApplyExtentUpdate();
	void ApplyMapTypeUpdate();
	void DestroyView();
	void Initialize();
	void InitializeMap();
	void InitializeMemorySystem();
	bool InsertAuthenticationConfig(const FGuid& InAuthConfigId, const FString& InSource);
	void InvalidateLayers();
	void PullFromGeoReferencingSystem();
	void PushToGeoReferencingSystem();
	void ResetMap();
	void SetMapToView();
	void UpdateAuthenticationConfigurations();
	void UpdateENUToWorldTransform();
	void UpdateLayers();
};
