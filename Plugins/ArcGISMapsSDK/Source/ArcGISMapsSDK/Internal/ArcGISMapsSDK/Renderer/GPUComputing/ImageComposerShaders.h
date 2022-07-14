#pragma once

#include "GlobalShader.h"
#include "PipelineStateCache.h"
#include "ShaderParameterMacros.h"
#include "ShaderParameterStruct.h"

class FImageComposerCS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FImageComposerCS);
	SHADER_USE_PARAMETER_STRUCT(FImageComposerCS, FGlobalShader);

	// clang-format off
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(uint32, NumTextures)
		SHADER_PARAMETER_ARRAY(FVector4f, OffsetScaleOpacity, [8])
		SHADER_PARAMETER_SAMPLER_ARRAY(SamplerState, Samplers, [8])
		SHADER_PARAMETER_TEXTURE_ARRAY(Texture2D, Textures, [8])
		SHADER_PARAMETER_UAV(RWTexture2D<float4>, Output)
	END_SHADER_PARAMETER_STRUCT()
	// clang-format on

public:
	static const uint32 THREADGROUP_SIZE_X;
	static const uint32 THREADGROUP_SIZE_Y;

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return RHISupportsComputeShaders(Parameters.Platform);
	}
};

struct FImageComposerElementVertex
{
	FVector4f Position;
	FVector2f TextureCoordinate;

	FImageComposerElementVertex()
	{
	}

	FImageComposerElementVertex(const FVector4f& InPosition, const FVector2D& InTextureCoordinate)
	: Position(InPosition),
	  TextureCoordinate(InTextureCoordinate)
	{
	}
};

class FImageComposerPSResources : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;
	FBufferRHIRef VertexBufferRHI;

	virtual ~FImageComposerPSResources()
	{
	}

	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint16 Stride = sizeof(FImageComposerElementVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FImageComposerElementVertex, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FImageComposerElementVertex, TextureCoordinate), VET_Float2, 1, Stride));
		VertexDeclarationRHI = PipelineStateCache::GetOrCreateVertexDeclaration(Elements);

		TResourceArray<FImageComposerElementVertex> Vertices;
		Vertices.AddUninitialized(4);

		Vertices[0].Position.Set(-1.0f, +1.0f, 1.0f, 1.0f); // Top Left
		Vertices[1].Position.Set(+1.0f, +1.0f, 1.0f, 1.0f); // Top Right
		Vertices[2].Position.Set(-1.0f, -1.0f, 1.0f, 1.0f); // Bottom Left
		Vertices[3].Position.Set(+1.0f, -1.0f, 1.0f, 1.0f); // Bottom Right

		Vertices[0].TextureCoordinate.Set(0.0f, 0.0f);
		Vertices[1].TextureCoordinate.Set(1.0f, 0.0f);
		Vertices[2].TextureCoordinate.Set(0.0f, 1.0f);
		Vertices[3].TextureCoordinate.Set(1.0f, 1.0f);

		FRHIResourceCreateInfo CreateInfo(TEXT("FImageComposerPSResources"), &Vertices);
		VertexBufferRHI = RHICreateVertexBuffer(sizeof(FImageComposerElementVertex) * 4, BUF_Static, CreateInfo);
	}

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
		VertexBufferRHI.SafeRelease();
	}
};

extern TGlobalResource<FImageComposerPSResources> GImageComposerPSResources;

class FImageComposerVS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FImageComposerVS);

public:
	FImageComposerVS();
	FImageComposerVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}
};

class FImageComposerPS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FImageComposerPS);
	SHADER_USE_PARAMETER_STRUCT(FImageComposerPS, FGlobalShader);

	// clang-format off
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE_ARRAY(Texture2D, Textures, [8])
		SHADER_PARAMETER_SAMPLER_ARRAY(SamplerState, Samplers, [8])
		SHADER_PARAMETER_ARRAY(FVector4f, OffsetScaleOpacity, [8])
		SHADER_PARAMETER(uint32, NumTextures)
	END_SHADER_PARAMETER_STRUCT()
	// clang-format on

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::ES3_1);
	}
};
