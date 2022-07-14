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
#include "ArcGISMapsSDK/Components/ArcGISMesh.h"

namespace
{
FBoxSphereBounds CalculateLocalBounds(TSharedPtr<const FGPUResourceMesh> InGPUResourceMesh)
{
	FBox Box{ForceInit};

	for (int32 i = 0; i < InGPUResourceMesh->Positions.Num(); ++i)
	{
		Box += FVector3d(InGPUResourceMesh->Positions[i]);
	}

	if (Box.IsValid)
	{
		return FBoxSphereBounds(Box);
	}
	else
	{
		// Fallback to reset box sphere bounds
		return FBoxSphereBounds(FVector3d(0, 0, 0), FVector3d(0, 0, 0), 0);
	}
}
} // namespace

void UArcGISMesh::BeginDestroy()
{
	Super::BeginDestroy();

	if (bRenderingResourcesInitialized)
	{
		ReleaseResources();
	}
}

FBoxSphereBounds UArcGISMesh::GetBounds() const
{
	return LocalBounds;
}

const FArcGISMeshRenderData* UArcGISMesh::GetRenderData() const
{
	return RenderData.Get();
}

bool UArcGISMesh::IsReadyForFinishDestroy()
{
	if (bRenderingResourcesInitialized)
	{
		ReleaseResources();
	}

	return ReleaseResourcesFence.IsFenceComplete();
}

void UArcGISMesh::ReleaseResources()
{
	if (RenderData)
	{
		RenderData->ReleaseResources();

		ReleaseResourcesFence.BeginFence();
	}

	bRenderingResourcesInitialized = false;
}

void UArcGISMesh::SetRenderData(TSharedPtr<const Esri::ArcGISMapsSDK::Renderer::GPUResources::FGPUResourceMesh> InGPUResourceMesh)
{
	if (!RenderData)
	{
		UWorld* World = GetWorld();

		RenderData = MakeUnique<FArcGISMeshRenderData>(World ? World->FeatureLevel.GetValue() : ERHIFeatureLevel::Num);
	}

	RenderData->SetMeshData(InGPUResourceMesh);

	LocalBounds = CalculateLocalBounds(std::move(InGPUResourceMesh));

	bRenderingResourcesInitialized = true;
}

int32 UArcGISMesh::GetFeatureIndexByTriangleIndex(uint32 InPrimitiveIndex) const
{
	return RenderData ? RenderData->GetFeatureIndexByTriangleIndex(InPrimitiveIndex) : -1;
}
