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

#include "Components/MeshComponent.h"

#include "ArcGISMapsSDK/API/GameEngine/View/ArcGISView.h"

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
class FArcGISRenderer
{
public:
	class Impl;

public:
	FArcGISRenderer(TSharedPtr<Esri::GameEngine::View::ArcGISView> InView,
					UObject* InOuter,
					bool bInAreMeshCollidersEnabled,
					bool bInIsTerrainOcclusionEnabled);
	~FArcGISRenderer();

	void Update();

	uint32 GetLayerIdByMeshComponent(const UMeshComponent* InMeshComponent) const;

	bool GetAreMeshCollidersEnabled() const;
	void SetAreMeshCollidersEnabled(bool bEnabled);

private:
	TUniquePtr<Impl> PImpl;
};
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
