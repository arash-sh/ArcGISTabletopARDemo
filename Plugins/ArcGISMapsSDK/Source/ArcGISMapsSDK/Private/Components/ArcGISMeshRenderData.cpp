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
#include "ArcGISMapsSDK/Components/ArcGISMeshRenderData.h"

namespace
{
inline void CopyGPUBufferLocked(FRHIBuffer* RHIVertexBuffer, void* Src, SIZE_T Size)
{
	void* VertexBufferData = RHILockBuffer(RHIVertexBuffer, 0, Size, RLM_WriteOnly);
	FMemory::Memcpy(VertexBufferData, Src, Size);
	RHIUnlockBuffer(RHIVertexBuffer);
}

inline void InitOrUpdateResource(FRenderResource* Resource)
{
	if (!Resource->IsInitialized())
	{
		Resource->InitResource();
	}
	else
	{
		Resource->UpdateRHI();
	}
}
} // namespace

FArcGISMeshRenderData::FArcGISMeshRenderData(ERHIFeatureLevel::Type InFeatureLevel) : VertexFactory(InFeatureLevel, "FArcGISMeshRenderData")
{
}

TArray<FVector3d> FArcGISMeshRenderData::CalculateColliderPositions() const
{
	TArray<FVector3d> CollisionPositions;
	CollisionPositions.SetNum(PositionVertexBuffer.GetNumVertices());

	for (uint32 i = 0; i < PositionVertexBuffer.GetNumVertices(); ++i)
	{
		CollisionPositions[i] = FVector3d(PositionVertexBuffer.VertexPosition(i));
	}

	return CollisionPositions;
}

void FArcGISMeshRenderData::FillBuffersFromGPUResourceMesh(TSharedPtr<const FGPUResourceMesh> GPUResourceMesh)
{
	static FVector4f XAxisTangent(1, 0, 0, 0);

	auto NumTriangles = GPUResourceMesh->Triangles.Num();
	auto NumPositions = GPUResourceMesh->Positions.Num();
	auto NumNormals = GPUResourceMesh->Normals.Num();
	auto NumColors = GPUResourceMesh->Colors.Num();

	FMemory::Memcpy(IndexBuffer.Indices.GetData(), GPUResourceMesh->Triangles.GetData(), NumTriangles * sizeof(int32));

	for (int32 i = 0; i < NumPositions; ++i)
	{
		PositionVertexBuffer.VertexPosition(i) = GPUResourceMesh->Positions[i];

		if (i < NumNormals)
		{
			const auto& TangentZ = GPUResourceMesh->Normals[i];
			const FVector4f* TangentX = &XAxisTangent;

			if (i < GPUResourceMesh->Tangents.Num())
			{
				TangentX = &GPUResourceMesh->Tangents[i];
			}

			const auto TangentY = FVector3f::CrossProduct(*TangentX, TangentZ) * ((*TangentX).W < 0 ? -1 : 1);
			StaticMeshVertexBuffer.SetVertexTangents(i, *TangentX, TangentY, TangentZ);
		}
		else
		{
			StaticMeshVertexBuffer.SetVertexTangents(i, FVector3f(0, 1, 0), FVector3f(1, 0, 0), FVector3f(0, 0, 1));
		}

		if (i < NumColors)
		{
			const auto& Color = GPUResourceMesh->Colors[i];

			const auto a = (Color & 0xff000000) >> 24;
			const auto r = (Color & 0x00ff0000) >> 16;
			const auto g = (Color & 0x0000ff00) >> 8;
			const auto b = (Color & 0x000000ff) >> 0;

			ColorVertexBuffer.VertexColor(i) = FColor(b, g, r, a);
		}
		else
		{
			ColorVertexBuffer.VertexColor(i) = FColor(0xFFFFFFFF);
		}
	}

	int32 NumTexCoords = FMath::Min(StaticMeshVertexBuffer.GetNumTexCoords(), (uint32)GPUResourceMesh->UVs.Num());

	for (int32 j = 0; j < NumTexCoords; ++j)
	{
		const auto& UVs = GPUResourceMesh->UVs[j];

		auto NumUVs = UVs.Dimensions == 1 || UVs.Dimensions == 2 ? static_cast<int32>(UVs.Elements.Num() / UVs.Dimensions) : 0;

		for (int32 i = 0; i < NumPositions; ++i)
		{
			FVector2f value = {0, 0};

			if (i < NumUVs)
			{
				if (UVs.Dimensions == 1)
				{
					value = {UVs.Elements[i], 0};
				}
				else if (UVs.Dimensions == 2)
				{
					value = {UVs.Elements[i * 2 + 0], UVs.Elements[i * 2 + 1]};
				}
			}

			StaticMeshVertexBuffer.SetVertexUV(i, j, value);
		}
	}

	MaxVertexIndex = PositionVertexBuffer.GetNumVertices() - 1;
	NumPrimitives = IndexBuffer.Indices.Num() / 3;
}

void FArcGISMeshRenderData::GetPhysicsTriMeshData(FTriMeshCollisionData* CollisionData) const
{
	CollisionData->Vertices.SetNum(PositionVertexBuffer.GetNumVertices());
	FMemory::Memcpy(CollisionData->Vertices.GetData(), PositionVertexBuffer.GetVertexData(),
					PositionVertexBuffer.GetNumVertices() * PositionVertexBuffer.GetStride());

	int ntriangles = IndexBuffer.Indices.Num() / 3;

	CollisionData->Indices.SetNum(ntriangles);
	CollisionData->MaterialIndices.SetNumZeroed(ntriangles);

	for (int i = 0; i < IndexBuffer.Indices.Num(); i += 3)
	{
		FTriIndices& triangle = CollisionData->Indices[i / 3];
		triangle.v0 = IndexBuffer.Indices[i];
		triangle.v1 = IndexBuffer.Indices[i + 1];
		triangle.v2 = IndexBuffer.Indices[i + 2];
	}
}

void FArcGISMeshRenderData::InitResources()
{
	ENQUEUE_RENDER_COMMAND(ArcGISMeshRenderDataInitResources)
	([&](FRHICommandListImmediate& RHICmdList) {
		constexpr int LightMapIndex = 0;

		InitOrUpdateResource(&PositionVertexBuffer);
		InitOrUpdateResource(&StaticMeshVertexBuffer);
		InitOrUpdateResource(&ColorVertexBuffer);

		FLocalVertexFactory::FDataType Data;
		{
			PositionVertexBuffer.BindPositionVertexBuffer(&VertexFactory, Data);
			StaticMeshVertexBuffer.BindTangentVertexBuffer(&VertexFactory, Data);
			StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer(&VertexFactory, Data);
			StaticMeshVertexBuffer.BindLightMapVertexBuffer(&VertexFactory, Data, LightMapIndex);
			ColorVertexBuffer.BindColorVertexBuffer(&VertexFactory, Data);
		}
		VertexFactory.SetData(Data);

		InitOrUpdateResource(&VertexFactory);
		InitOrUpdateResource(&IndexBuffer);
	});

	bIsInitialized = true;
}

bool FArcGISMeshRenderData::IsInitialized() const
{
	return bIsInitialized;
}

void FArcGISMeshRenderData::ReleaseResources()
{
	bIsInitialized = false;

	BeginReleaseResource(&PositionVertexBuffer);
	BeginReleaseResource(&StaticMeshVertexBuffer);
	BeginReleaseResource(&ColorVertexBuffer);
	BeginReleaseResource(&IndexBuffer);
	BeginReleaseResource(&VertexFactory);
}

void FArcGISMeshRenderData::ResizeBuffers(TSharedPtr<const FGPUResourceMesh> GPUResourceMesh)
{
	auto NumTriangles = GPUResourceMesh->Triangles.Num();
	auto NumVertices = GPUResourceMesh->Positions.Num();
	auto NumUVs = GPUResourceMesh->UVs.Num();

	IndexBuffer.Indices.SetNum(NumTriangles);

	if (PositionVertexBuffer.GetNumVertices() != NumVertices)
	{
		PositionVertexBuffer.Init(NumVertices);
	}

	if (StaticMeshVertexBuffer.GetNumVertices() != NumVertices || StaticMeshVertexBuffer.GetNumTexCoords() != NumUVs ||
		StaticMeshVertexBuffer.GetUseFullPrecisionUVs() != GPUResourceMesh->bUseFullPrecisionUVs)
	{
		StaticMeshVertexBuffer.SetUseFullPrecisionUVs(GPUResourceMesh->bUseFullPrecisionUVs);
		StaticMeshVertexBuffer.SetUseHighPrecisionTangentBasis(true);

		// Init depends on the precision config
		StaticMeshVertexBuffer.Init(NumVertices, NumUVs);
	}

	if (ColorVertexBuffer.GetNumVertices() != NumVertices)
	{
		ColorVertexBuffer.Init(NumVertices);
	}
}

void FArcGISMeshRenderData::SetMeshData(TSharedPtr<const FGPUResourceMesh> InGPUResourceMesh)
{
	ResizeBuffers(InGPUResourceMesh);

	FillBuffersFromGPUResourceMesh(std::move(InGPUResourceMesh));

	if (bIsInitialized)
	{
		UpdateResources();
	}
	else
	{
		InitResources();
	}
}

int32 FArcGISMeshRenderData::GetFeatureIndexByTriangleIndex(uint32 InPrimitiveIndex) const
{
	if (3 * static_cast<int32>(InPrimitiveIndex) < IndexBuffer.Indices.Num() && StaticMeshVertexBuffer.GetNumTexCoords() > 1)
	{
		if (auto vertexIndex = IndexBuffer.Indices[3 * InPrimitiveIndex]; vertexIndex < StaticMeshVertexBuffer.GetNumVertices())
		{
			return static_cast<int32>(StaticMeshVertexBuffer.GetVertexUV(vertexIndex, 1).X);
		}
	}

	return -1;
}

void FArcGISMeshRenderData::SetupMeshBatch(FMeshBatch& MeshBatch) const
{
	MeshBatch.VertexFactory = &VertexFactory;

	FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

	BatchElement.FirstIndex = 0;
	BatchElement.IndexBuffer = &IndexBuffer;
	BatchElement.MaxVertexIndex = MaxVertexIndex;
	BatchElement.MinVertexIndex = 0;
	BatchElement.NumPrimitives = NumPrimitives;
}

void FArcGISMeshRenderData::UpdateResources()
{
	ENQUEUE_RENDER_COMMAND(ArcGISMeshRenderDataUpdateResources)
	([&](FRHICommandListImmediate& RHICmdList) {
		CopyGPUBufferLocked(PositionVertexBuffer.VertexBufferRHI, PositionVertexBuffer.GetVertexData(),
							PositionVertexBuffer.GetNumVertices() * PositionVertexBuffer.GetStride());
		CopyGPUBufferLocked(ColorVertexBuffer.VertexBufferRHI, ColorVertexBuffer.GetVertexData(),
							ColorVertexBuffer.GetNumVertices() * ColorVertexBuffer.GetStride());
		CopyGPUBufferLocked(StaticMeshVertexBuffer.TangentsVertexBuffer.VertexBufferRHI, StaticMeshVertexBuffer.GetTangentData(),
							StaticMeshVertexBuffer.GetTangentSize());
		CopyGPUBufferLocked(StaticMeshVertexBuffer.TexCoordVertexBuffer.VertexBufferRHI, StaticMeshVertexBuffer.GetTexCoordData(),
							StaticMeshVertexBuffer.GetTexCoordSize());

		MaxVertexIndex = PositionVertexBuffer.GetNumVertices() - 1;
		NumPrimitives = IndexBuffer.Indices.Num() / 3;
	});
}
