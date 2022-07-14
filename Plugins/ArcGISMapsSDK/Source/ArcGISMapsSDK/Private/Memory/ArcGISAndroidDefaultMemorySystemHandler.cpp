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
#include "ArcGISMapsSDK/Memory/ArcGISAndroidDefaultMemorySystemHandler.h"

namespace
{
FArcGISAndroidDefaultMemorySystemHandler* AndroidDefaultMemorySystemHandler{nullptr};
FCriticalSection AndroidDefaultMemorySystemHandlerLock;
} // namespace

FArcGISAndroidDefaultMemorySystemHandler::FArcGISAndroidDefaultMemorySystemHandler()
{
	FScopeLock ScopeLock(&AndroidDefaultMemorySystemHandlerLock);

	AndroidDefaultMemorySystemHandler = this;
}

FArcGISAndroidDefaultMemorySystemHandler::~FArcGISAndroidDefaultMemorySystemHandler()
{
	FScopeLock ScopeLock(&AndroidDefaultMemorySystemHandlerLock);

	AndroidDefaultMemorySystemHandler = nullptr;
}

void FArcGISAndroidDefaultMemorySystemHandler::NotifyLowMemoryWarning()
{
	FScopeLock ScopeLock(&AndroidDefaultMemorySystemHandlerLock);

	if (AndroidDefaultMemorySystemHandler)
	{
		AndroidDefaultMemorySystemHandler->MemorySystem->NotifyLowMemoryWarning();
	}
}
