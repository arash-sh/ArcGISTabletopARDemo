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
#include "Materials/MaterialInterface.h"
#include "PrimitiveSceneProxy.h"

#include "ArcGISMapsSDK/Components/ArcGISMeshComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISMeshRenderData.h"

class FArcGISMeshSceneProxy final : public FPrimitiveSceneProxy
{
public:
	FArcGISMeshSceneProxy(const UArcGISMeshComponent* InComponent, bool bIsVisible);

	// FPrimitiveSceneProxy
	bool CanBeOccluded() const override;
	void GetDynamicMeshElements(const TArray<const FSceneView*>& Views,
								const FSceneViewFamily& ViewFamily,
								uint32 VisibilityMap,
								FMeshElementCollector& Collector) const override;
	uint32 GetMemoryFootprint(void) const override;
	virtual SIZE_T GetTypeHash() const override;
	FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	uint32 GetAllocatedSize(void) const;
	void SetInternalVisibility(bool InIsVisible);

private:
	void SetupMeshBatch(FMeshBatch& MeshBatch, FMaterialRenderProxy* Material, bool bWireframe) const;
	void SetupMeshBatchElement(FMeshBatchElement& BatchElement, FMeshElementCollector& Collector) const;

	bool bIsVisible;

	UMaterialInterface* MaterialInterface;
	FMaterialRelevance MaterialRelevance;
	const FArcGISMeshRenderData* RenderData{nullptr};
};
