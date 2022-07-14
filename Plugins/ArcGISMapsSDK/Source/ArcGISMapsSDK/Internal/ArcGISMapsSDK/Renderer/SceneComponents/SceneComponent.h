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

#include "ArcGISMapsSDK/Components/ArcGISMeshComponent.h"
#include "ArcGISMapsSDK/Renderer/GPUResources/GPUResourceMaterial.h"

struct ObbTransform
{
	FVector3d pos;
	FVector3d scale;
	FQuat rot;
};

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
namespace SceneComponents
{
class FSceneComponent final : public TSharedFromThis<FSceneComponent>
{
public:
	FSceneComponent(UObject* InOuter, uint32 InLayerId);
	~FSceneComponent();

	FVector3d GetLocation();

	void SetLocation(const FVector3d& NewLocation);
	void SetRotation(const FQuat& NewRotation);
	void SetScale(const FVector3d& NewScale);
	void SetMaterial(const TSharedPtr<const GPUResources::FGPUResourceMaterial>& GPUResourceMaterial);
	void SetMesh(const TSharedPtr<const GPUResources::FGPUResourceMesh>& GPUResourceMesh);
	void SetShadows(bool bUseShadows);
	void SetVisible(bool bIsVisible);
	inline bool GetVisible(){ return bVisible; }

	bool GetIsMeshColliderEnabled() const;
	void SetIsMeshColliderEnabled(bool bEnabled);

	bool GetMasksTerrain() const;
	void SetMasksTerrain(bool bClipsTerrain);

	const ObbTransform& GetObbTransform();
	void SetObbTransform(const ObbTransform& InMatrix);

	uint32 GetLayerId() const;

	const UArcGISMeshComponent* GetMeshComponent() const;

	const FTransform& GetTransform()
	{
		return ArcGISMeshComponent->GetComponentTransform();
	}

	std::pair<FPositionVertexBuffer*, FDynamicMeshIndexBuffer32*> GetPositionBuffer()
	{
		if (ArcGISMeshComponent)
		{
			return ArcGISMeshComponent->GetPositionBuffer();
		}
		return { nullptr, nullptr };
	}

private:
	void CreateMeshComponent();

	bool bShadows = true;
	bool bVisible = false;
	bool bClipsTerrain = false;

	uint32 LayerId = 0;

	UArcGISMeshComponent* ArcGISMeshComponent = nullptr;
	UObject* Outer;

	ObbTransform ObbTransform;
};
} // namespace SceneComponents
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
