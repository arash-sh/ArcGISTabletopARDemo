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

#if defined(TRACY_ENABLE)
#include "tracy/Tracy.hpp"

#define TRACE_SCOPE(scope_name) ZoneScopedN(scope_name)

#define TRACE_FRAME_BEGIN(frame_name) FrameMarkStart(frame_name)
#define TRACE_FRAME_END(frame_name) FrameMarkEnd(frame_name)
#else
#define TRACE_SCOPE(scope_name)

#define TRACE_FRAME_BEGIN(frame_name)
#define TRACE_FRAME_END(frame_name)
#endif
