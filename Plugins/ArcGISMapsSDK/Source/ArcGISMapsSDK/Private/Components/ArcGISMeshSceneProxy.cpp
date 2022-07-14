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
#include "ArcGISMapsSDK/Components/ArcGISMeshSceneProxy.h"

#include "Engine.h"
#include "Materials/Material.h"
#include "Misc/EngineVersionComparison.h"

FArcGISMeshSceneProxy::FArcGISMeshSceneProxy(const UArcGISMeshComponent* InComponent, bool InIsVisible)
: FPrimitiveSceneProxy(InComponent),
  bIsVisible(InIsVisible),
  MaterialRelevance(InComponent->GetMaterialRelevance(GetScene().GetFeatureLevel())),
  RenderData(InComponent->GetRenderData())
{
	check(RenderData);
	checkf(RenderData->IsInitialized(), TEXT("Uninitialized RenderData for ArcGISMesh"));

	MaterialInterface = InComponent->GetMaterial(0);

	if (!MaterialInterface)
	{
		MaterialInterface = UMaterial::GetDefaultMaterial(MD_Surface);
	}
}

bool FArcGISMeshSceneProxy::CanBeOccluded() const
{
	return !MaterialRelevance.bDisableDepthTest;
}

uint32 FArcGISMeshSceneProxy::GetAllocatedSize() const
{
	return FPrimitiveSceneProxy::GetAllocatedSize();
}

void FArcGISMeshSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
												   const FSceneViewFamily& ViewFamily,
												   uint32 VisibilityMap,
												   FMeshElementCollector& Collector) const
{
	if (!bIsVisible)
	{
		return;
	}

	// Set up wireframe material (if needed)
	const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

	FMaterialRenderProxy* MaterialProxy = nullptr;

	if (bWireframe)
	{
		MaterialProxy = new FColoredMaterialRenderProxy(GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL,
														FLinearColor(0, 0.5f, 1.f));
		Collector.RegisterOneFrameMaterialProxy(MaterialProxy);
	}
	else
	{
		MaterialProxy = MaterialInterface->GetRenderProxy();
	}

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];

			FMeshBatch& MeshBatch = Collector.AllocateMesh();

			SetupMeshBatch(MeshBatch, MaterialProxy, bWireframe);
			SetupMeshBatchElement(MeshBatch.Elements[0], Collector);

			Collector.AddMesh(ViewIndex, MeshBatch);

			if (ViewFamily.EngineShowFlags.Bounds)
			{
				const ESceneDepthPriorityGroup DrawBoundsDPG = SDPG_World;
				const auto& PDI = Collector.GetPDI(ViewIndex);

				DrawWireBox(PDI, GetBounds().GetBox(), FColor(72, 72, 255), DrawBoundsDPG);
				DrawWireBox(PDI, GetLocalToWorld(), GetLocalBounds().GetBox(), FColor(255, 72, 72), DrawBoundsDPG);
			}
		}
	}
}

uint32 FArcGISMeshSceneProxy::GetMemoryFootprint(void) const
{
	return sizeof(*this) + GetAllocatedSize();
}

SIZE_T FArcGISMeshSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;

	return reinterpret_cast<size_t>(&UniquePointer);
}

FPrimitiveViewRelevance FArcGISMeshSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance Result;
	Result.bDrawRelevance = IsShown(View);
	Result.bShadowRelevance = IsShadowCast(View);
	Result.bDynamicRelevance = true;
	Result.bRenderInMainPass = ShouldRenderInMainPass();
	Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
	Result.bRenderCustomDepth = ShouldRenderCustomDepth();
	Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;
	MaterialRelevance.SetPrimitiveViewRelevance(Result);
	Result.bVelocityRelevance = DrawsVelocity() && Result.bOpaque && Result.bRenderInMainPass;
	return Result;
}

void FArcGISMeshSceneProxy::SetInternalVisibility(bool InIsVisible)
{
	ENQUEUE_RENDER_COMMAND(FRenderMeshSetInternalVisibility)
	([this, InIsVisible](FRHICommandListImmediate& RHICmdList) {
		bIsVisible = InIsVisible;
	});
}

void FArcGISMeshSceneProxy::SetupMeshBatch(FMeshBatch& MeshBatch, FMaterialRenderProxy* Material, bool bWireframe) const
{
	RenderData->SetupMeshBatch(MeshBatch);

	MeshBatch.bCanApplyViewModeOverrides = true;
	MeshBatch.bWireframe = bWireframe;

	MeshBatch.CastShadow = true;
	MeshBatch.DepthPriorityGroup = SDPG_World;
	MeshBatch.MaterialRenderProxy = Material;
	MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
	MeshBatch.Type = PT_TriangleList;
}

void FArcGISMeshSceneProxy::SetupMeshBatchElement(FMeshBatchElement& BatchElement, FMeshElementCollector& Collector) const
{
	bool bHasPrecomputedVolumetricLightmap;
	FMatrix PreviousLocalToWorld;
	int32 SingleCaptureIndex;
	bool bOutputVelocity;
	GetScene().GetPrimitiveUniformShaderParameters_RenderThread(GetPrimitiveSceneInfo(), bHasPrecomputedVolumetricLightmap, PreviousLocalToWorld,
																SingleCaptureIndex, bOutputVelocity);

	FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();

	DynamicPrimitiveUniformBuffer.Set(GetLocalToWorld(), PreviousLocalToWorld, GetBounds(), GetLocalBounds(), true, bHasPrecomputedVolumetricLightmap,
									  DrawsVelocity(), bOutputVelocity);
	BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;
}
