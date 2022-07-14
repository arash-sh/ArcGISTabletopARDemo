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

#include "ArcGISMapsSDK/Components/ArcGISMeshRenderData.h"

#include "ArcGISMesh.generated.h"

UCLASS()
class UArcGISMesh : public UObject
{
	GENERATED_BODY()

public:
	// UObject
	void BeginDestroy() override;
	bool IsReadyForFinishDestroy() override;

	FBoxSphereBounds GetBounds() const;
	const FArcGISMeshRenderData* GetRenderData() const;
	void SetRenderData(TSharedPtr<const Esri::ArcGISMapsSDK::Renderer::GPUResources::FGPUResourceMesh> InGPUResourceMesh);

	int32 GetFeatureIndexByTriangleIndex(uint32 InPrimitiveIndex) const;

	std::pair<FPositionVertexBuffer*, FDynamicMeshIndexBuffer32*> GetPositionBuffer()
	{
		if (RenderData)
		{
			return RenderData->GetPositionBuffer();
		}
		return { nullptr, nullptr };
	}
private:
	void ReleaseResources();

	bool bRenderingResourcesInitialized{false};

	FBoxSphereBounds LocalBounds;
	FRenderCommandFence ReleaseResourcesFence;
	TUniquePtr<FArcGISMeshRenderData> RenderData;
};
