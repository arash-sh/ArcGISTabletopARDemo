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
#include "ArcGISMapsSDK/Components/ArcGISMeshComponent.h"

#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/PhysicsSettings.h"

#include "ArcGISMapsSDK/Components/ArcGISMeshSceneProxy.h"

UArcGISMeshComponent::UArcGISMeshComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FBoxSphereBounds UArcGISMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	if (!ArcGISMesh)
	{
		return FBoxSphereBounds(FBox(ForceInit));
	}

	FBoxSphereBounds Ret = ArcGISMesh->GetBounds().TransformBy(LocalToWorld);

	// 1.1 is a walkaround for tiles flickering when near the viewport limits
	Ret.BoxExtent *= BoundsScale * 1.1;
	Ret.SphereRadius *= BoundsScale * 1.1;

	return Ret;
}

FPrimitiveSceneProxy* UArcGISMeshComponent::CreateSceneProxy()
{
	if (!ArcGISMesh)
	{
		return nullptr;
	}

	return new FArcGISMeshSceneProxy(this, bIsVisible);
}

const FArcGISMeshRenderData* UArcGISMeshComponent::GetRenderData() const
{
	return ArcGISMesh->GetRenderData();
}

void UArcGISMeshComponent::SetInternalVisibility(bool InIsVisible)
{
	bIsVisible = InIsVisible;

	if (SceneProxy && !IsRenderStateDirty())
	{
		auto ArcGISMeshSceneProxy = static_cast<FArcGISMeshSceneProxy*>(SceneProxy);

		ArcGISMeshSceneProxy->SetInternalVisibility(bIsVisible);
	}

	if (bIsMeshColliderEnabled && bMeshColliderCreated)
	{
		UpdateColliderState(bIsVisible);
	}
}

int32 UArcGISMeshComponent::GetNumMaterials() const
{
	return 1;
}

void UArcGISMeshComponent::SetMesh(TSharedPtr<const FGPUResourceMesh> InGPUResourceMesh)
{
	if (ArcGISMesh == nullptr)
	{
		ArcGISMesh = NewObject<UArcGISMesh>(this);
	}

	ArcGISMesh->SetRenderData(InGPUResourceMesh);

	if (IsRenderStateCreated())
	{
		// This will eventually trigger a RecreateRenderState_Concurrent
		MarkRenderStateDirty();

		UpdateBounds();
	}
	else if (ShouldCreateRenderState())
	{
		RecreateRenderState_Concurrent();
	}

	bMeshColliderCreated = false;

	if (bIsMeshColliderEnabled)
	{
		CreateCollider();
	}
}

int32 UArcGISMeshComponent::GetFeatureIndexByTriangleIndex(uint32 InPrimitiveIndex) const
{
	return ArcGISMesh ? ArcGISMesh->GetFeatureIndexByTriangleIndex(InPrimitiveIndex) : -1;
}

bool UArcGISMeshComponent::ShouldCreateRenderState() const
{
	if (!Super::ShouldCreateRenderState())
	{
		return false;
	}

	if (ArcGISMesh == nullptr || ArcGISMesh->GetRenderData() == nullptr)
	{
		return false;
	}

	return true;
}

bool UArcGISMeshComponent::ContainsPhysicsTriMeshData(bool bInUseAllTriData) const
{
	return bIsMeshColliderEnabled;
}

UBodySetup* UArcGISMeshComponent::GetBodySetup()
{
	return BodySetup;
}

UMaterialInterface* UArcGISMeshComponent::GetMaterialFromCollisionFaceIndex(int32 FaceIndex, int32& SectionIndex) const
{
	return nullptr;
}

bool UArcGISMeshComponent::WantsNegXTriMesh()
{
	return false;
}

void UArcGISMeshComponent::UpdateColliderPositions()
{
	if (bMeshColliderCreated)
	{
		BodyInstance.UpdateTriMeshVertices(ArcGISMesh->GetRenderData()->CalculateColliderPositions());
	}
}

void UArcGISMeshComponent::CreateCollider()
{
	UWorld* World = GetWorld();

	if (World && World->IsGameWorld() && ArcGISMesh)
	{
		for (UBodySetup* OldBody : AsyncBodySetupQueue)
		{
			OldBody->AbortPhysicsMeshAsyncCreation();
		}

		// The body setup in a template needs to be public since the property is instanced and thus is the archetype of the instance meaning there is a direct reference
		auto NewBodySetup = NewObject<UBodySetup>(this, NAME_None, (IsTemplate() ? RF_Public : RF_NoFlags));

		NewBodySetup->BodySetupGuid = FGuid::NewGuid();
		NewBodySetup->bGenerateMirroredCollision = false;
		NewBodySetup->bDoubleSidedGeometry = true;
		NewBodySetup->CollisionTraceFlag = CTF_UseComplexAsSimple;

		AsyncBodySetupQueue.Add(NewBodySetup);

		NewBodySetup->CreatePhysicsMeshesAsync(
			FOnAsyncPhysicsCookFinished::CreateUObject(this, &UArcGISMeshComponent::FinishPhysicsAsyncCook, NewBodySetup));
	}
}

void UArcGISMeshComponent::FinishPhysicsAsyncCook(bool bSuccess, UBodySetup* FinishedBodySetup)
{
	TArray<UBodySetup*> NewQueue;
	NewQueue.Reserve(AsyncBodySetupQueue.Num());

	int32 FoundIdx;
	if (AsyncBodySetupQueue.Find(FinishedBodySetup, FoundIdx))
	{
		if (bSuccess)
		{
			bMeshColliderCreated = true;

			// The new body was found in the array meaning it's newer so use it
			BodySetup = FinishedBodySetup;
			RecreatePhysicsState();

			// Remove any async body setups that were requested before this one
			for (int32 AsyncIdx = FoundIdx + 1; AsyncIdx < AsyncBodySetupQueue.Num(); ++AsyncIdx)
			{
				NewQueue.Add(AsyncBodySetupQueue[AsyncIdx]);
			}

			AsyncBodySetupQueue = NewQueue;
		}
		else
		{
			AsyncBodySetupQueue.RemoveAt(FoundIdx);
		}

		UpdateColliderState(bSuccess && bIsVisible && bIsMeshColliderEnabled);
	}
}

bool UArcGISMeshComponent::GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData, bool bInUseAllTriData)
{
	ArcGISMesh->GetRenderData()->GetPhysicsTriMeshData(CollisionData);

	CollisionData->bFlipNormals = true;
	CollisionData->bDeformableMesh = true;
	CollisionData->bFastCook = true;

	return true;
}

bool UArcGISMeshComponent::GetIsMeshColliderEnabled() const
{
	return bIsMeshColliderEnabled;
}

void UArcGISMeshComponent::SetIsMeshColliderEnabled(bool bEnabled)
{
	bIsMeshColliderEnabled = bEnabled;

	if (!bMeshColliderCreated && bEnabled)
	{
		CreateCollider();
	}
	else
	{
		UpdateColliderState(bEnabled);
	}
}

void UArcGISMeshComponent::UpdateColliderState(bool bEnabled)
{
	ECollisionEnabled::Type NewType = bEnabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;

	if (NewType != BodyInstance.GetCollisionEnabled())
	{
		SetCollisionEnabled(NewType);

		if (bEnabled && bDirtyCollisionPositions && ArcGISMesh)
		{
			UpdateColliderPositions();
		}
	}
}
