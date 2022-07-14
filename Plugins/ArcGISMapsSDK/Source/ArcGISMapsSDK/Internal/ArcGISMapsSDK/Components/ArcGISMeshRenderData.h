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
#include "DynamicMeshBuilder.h"
#include "Interface_CollisionDataProviderCore.h"
#include "Rendering/ColorVertexBuffer.h"
#include "Rendering/PositionVertexBuffer.h"
#include "Rendering/StaticMeshVertexBuffer.h"

#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceMesh.h"

using namespace Esri::ArcGISMapsSDK::Renderer::GPUResources;

class FArcGISMeshRenderData
{
public:
	FArcGISMeshRenderData(ERHIFeatureLevel::Type InFeatureLevel);

	TArray<FVector3d> CalculateColliderPositions() const;
	void GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData) const;
	bool IsInitialized() const;
	void ReleaseResources();
	void ResizeBuffers(TSharedPtr<const FGPUResourceMesh> InGPUResourceMesh);
	void SetupMeshBatch(FMeshBatch& MeshBatch) const;
	void SetMeshData(TSharedPtr<const FGPUResourceMesh> InGPUResourceMesh);

	int32 GetFeatureIndexByTriangleIndex(uint32 InPrimitiveIndex) const;

	std::pair<FPositionVertexBuffer*, FDynamicMeshIndexBuffer32*> GetPositionBuffer()
	{
		return { &PositionVertexBuffer, &IndexBuffer };
	}
private:
	void FillBuffersFromGPUResourceMesh(TSharedPtr<const FGPUResourceMesh> InGPUResourceMesh);
	void InitResources();
	void UpdateResources();

	bool bIsInitialized{false};

	uint32 NumPrimitives;
	uint32 MaxVertexIndex;

	FColorVertexBuffer ColorVertexBuffer;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FPositionVertexBuffer PositionVertexBuffer;
	FStaticMeshVertexBuffer StaticMeshVertexBuffer;
	FLocalVertexFactory VertexFactory;
};
