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

#include "Components/MeshComponent.h"
#include "CoreMinimal.h"
#include "Interfaces/Interface_CollisionDataProvider.h"

#include "ArcGISMapsSDK/Components/ArcGISMesh.h"

#include "ArcGISMeshComponent.generated.h"

UCLASS()
class UArcGISMeshComponent : public UMeshComponent, public IInterface_CollisionDataProvider
{
	GENERATED_BODY()

public:
	UArcGISMeshComponent(const FObjectInitializer& ObjectInitializer);

	const FArcGISMeshRenderData* GetRenderData() const;
	void SetInternalVisibility(bool InIsVisible);
	void SetMesh(TSharedPtr<const FGPUResourceMesh> InGPUResourceMesh);

	int32 GetFeatureIndexByTriangleIndex(uint32 InPrimitiveIndex) const;

	bool GetIsMeshColliderEnabled() const;
	void SetIsMeshColliderEnabled(bool bEnabled);

	std::pair<FPositionVertexBuffer*, FDynamicMeshIndexBuffer32*> GetPositionBuffer()
	{
		if (ArcGISMesh)
		{
			return ArcGISMesh->GetPositionBuffer();
		}
		return { nullptr, nullptr };
	}

private:
	// ActorComponent
	virtual bool ShouldCreateRenderState() const override;

	// USceneComponent
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	// UPrimitiveComponent
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual UBodySetup* GetBodySetup() override;
	virtual int32 GetNumMaterials() const override;

	// IInterface_CollisionDataProvider
	virtual bool GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData, bool bInUseAllTriData) override;
	virtual bool ContainsPhysicsTriMeshData(bool bInUseAllTriData) const override;
	virtual bool WantsNegXTriMesh() override;
	virtual UMaterialInterface* GetMaterialFromCollisionFaceIndex(int32 FaceIndex, int32& SectionIndex) const override;

	void CreateCollider();
	void FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup);
	void UpdateColliderPositions();
	void UpdateColliderState(bool bEnabled);

	UPROPERTY()
	TObjectPtr<UArcGISMesh> ArcGISMesh{nullptr};

	UPROPERTY()
	TArray<TObjectPtr<UBodySetup>> AsyncBodySetupQueue;

	UPROPERTY()
	TObjectPtr<UBodySetup> BodySetup{nullptr};

	bool bDirtyCollisionPositions{false};
	bool bIsMeshColliderEnabled{false};
	bool bIsVisible{true};
	bool bMeshColliderCreated{false};
};
