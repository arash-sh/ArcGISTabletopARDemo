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
#include "ArcGISMapsSDK/Renderer/ArcGISRenderCommandThrottle.h"

#include "Misc/EngineVersionComparison.h"

#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISDataBufferView.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/ArcGISRenderCommandType.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMeshBufferCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetMeshCommandParameters.h"
#include "ArcGISMapsSDK/API/GameEngine/RenderCommandQueue/Parameters/ArcGISSetTexturePixelDataCommandParameters.h"

namespace
{
template <typename TView>
void TSum(const TView& View, uint64& InOutSum)
{
	InOutSum += View.Size;
}

template <typename... TViews>
uint64 TResourceMemory(const TViews&... Views)
{
	uint64 Total{0};
	int dummy[] = {0, ((void)TSum(Views, Total), 0)...};
	return Total;
}

// Debugging: Stats for each "throttle"
//#define COLLECT_STATS
#ifdef COLLECT_STATS
struct FTimedStats
{
	uint32 CommandCount{0};
	uint32 ComposeCount{0};
	float MemoryMb{0};
	float ExecMs{0};
};
std::vector<FTimedStats> CollectedStats;
#endif

} // namespace

namespace Esri
{
namespace ArcGISMapsSDK
{
namespace Renderer
{
// Debugging: Total number of throttles happening in a session
uint32 FArcGISRenderCommandThrottle::ThrottleCount{0};

FArcGISRenderCommandThrottle::FArcGISRenderCommandThrottle() : StartTime(std::chrono::steady_clock::now())
{
#ifdef COLLECT_STATS
	CollectedStats.reserve(5000);
#endif
}

FArcGISRenderCommandThrottle::~FArcGISRenderCommandThrottle()
{
#ifdef COLLECT_STATS
	if (Stats.CommandCount > 0)
	{
		const auto ElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - StartTime);
		FTimedStats TimedStats{Stats.CommandCount, Stats.ComposeCount, Stats.ResourceMemory / (1024.f * 1024.f), ElapsedTime.count() / 1000.f};
		CollectedStats.emplace_back(std::move(TimedStats));
	}
#endif
}

bool FArcGISRenderCommandThrottle::DoThrottle(const GameEngine::RenderCommandQueue::ArcGISRenderCommand& RenderCommand)
{
	using namespace GameEngine::RenderCommandQueue;

	// Update stats from selected commands
	++Stats.CommandCount;
	switch (RenderCommand.GetType())
	{
		case ArcGISRenderCommandType::MultipleCompose:
		case ArcGISRenderCommandType::Compose:
		{
			++Stats.ComposeCount;
			break;
		}

		case ArcGISRenderCommandType::SetTexturePixelData:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetTexturePixelDataCommandParameters>();
			Stats.ResourceMemory += TResourceMemory(Params.Pixels);
			break;
		}

		case ArcGISRenderCommandType::SetMesh:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMeshCommandParameters>();
			Stats.ResourceMemory += TResourceMemory(Params.Colors, Params.FeatureIndices, Params.Normals, Params.Positions, Params.Tangents,
													Params.Triangles, Params.UvRegionIds, Params.Uvs);
			Stats.Triangles = Params.Triangles.Size / 12;
			break;
		}

		case ArcGISRenderCommandType::SetMeshBuffer:
		{
			const auto& Params = RenderCommand.GetParameters<Parameters::ArcGISSetMeshBufferCommandParameters>();
			Stats.ResourceMemory += TResourceMemory(Params.Buffer);
			break;
		}

		case ArcGISRenderCommandType::CommandGroupBegin:
		{
			Stats.bIsExecutingGroup = true;
			break;
		}

		case ArcGISRenderCommandType::CommandGroupEnd:
		{
			Stats.bIsExecutingGroup = false;
			break;
		}

		default:
			break;
	}

	// Determine whether to throttle at this command
	if (!bEnableThrottle || Stats.bIsExecutingGroup)
	{
		return false;
	}
	if (Stats.ComposeCount >= MaxComposeCommands || Stats.ResourceMemory >= MaxResourceMemory || Stats.Triangles >= MaxTriangles)
	{
		++ThrottleCount;
		return true;
	}
	const auto ElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - StartTime);
	if (ElapsedTime.count() >= MaxExecTimeMicroseconds)
	{
		++ThrottleCount;
		return true;
	}
	return false;
}
} // namespace Renderer
} // namespace ArcGISMapsSDK
} // namespace Esri
