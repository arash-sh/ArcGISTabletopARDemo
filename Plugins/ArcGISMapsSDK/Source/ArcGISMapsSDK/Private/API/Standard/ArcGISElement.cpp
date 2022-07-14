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
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISAttribute.h"
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISVisualizationAttribute.h"
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISVisualizationAttributeDescription.h"
#include "ArcGISMapsSDK/API/GameEngine/Security/ArcGISAuthenticationConfiguration.h"
#include "ArcGISMapsSDK/API/Standard/ArcGISElement.h" // IWYU pragma: associated
#include "ArcGISMapsSDK/API/Standard/ArcGISElementType.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISErrorHandler.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISException.h"
#include "ArcGISMapsSDK/CAPI/Standard/Element.h"

namespace Esri
{
namespace Standard
{
ArcGISElement::ArcGISElement()
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    SetHandle(RT_Element_create(&errorHandler));
}

ArcGISElementType ArcGISElement::GetObjectType() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getObjectType(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return static_cast<ArcGISElementType>(localResult);
}

void ArcGISElement::Clear()
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_clear(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
}

bool ArcGISElement::Equals(const ArcGISElement& element2) const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localElement2 = static_cast<const RT_ElementHandle>(element2.GetHandle());
    
    auto localResult = RT_Element_equals(reinterpret_cast<RT_ElementHandle>(GetHandle()), localElement2, &errorHandler);
    
    return localResult;
}

ArcGISElement ArcGISElement::FromArcGISAuthenticationConfiguration(const GameEngine::Security::ArcGISAuthenticationConfiguration& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localValue = static_cast<const RT_ArcGISAuthenticationConfigurationHandle>(value.GetHandle());
    
    auto localResult = RT_Element_fromArcGISAuthenticationConfiguration(localValue, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromArcGISAttribute(const GameEngine::Attributes::ArcGISAttribute& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localValue = static_cast<const RT_AttributeHandle>(value.GetHandle());
    
    auto localResult = RT_Element_fromAttribute(localValue, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromBool(bool value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromBool(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromFloat32(float value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromFloat32(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromFloat64(double value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromFloat64(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromInt16(int16_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromInt16(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromInt32(int32_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromInt32(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromInt64(int64_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromInt64(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromInt8(int8_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromInt8(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromString(const FString& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromString(TCHAR_TO_ANSI(*value), &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromUInt16(uint16_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromUInt16(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromUInt32(uint32_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromUInt32(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromUInt64(uint64_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromUInt64(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromUInt8(uint8_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_fromUInt8(value, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromArcGISVisualizationAttribute(const GameEngine::Attributes::ArcGISVisualizationAttribute& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localValue = static_cast<const RT_VisualizationAttributeHandle>(value.GetHandle());
    
    auto localResult = RT_Element_fromVisualizationAttribute(localValue, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

ArcGISElement ArcGISElement::FromArcGISVisualizationAttributeDescription(const GameEngine::Attributes::ArcGISVisualizationAttributeDescription& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localValue = static_cast<const RT_VisualizationAttributeDescriptionHandle>(value.GetHandle());
    
    auto localResult = RT_Element_fromVisualizationAttributeDescription(localValue, &errorHandler);
    
    auto localLocalResult = ArcGISElement{};
    
    if (localResult != nullptr)
    {
        localLocalResult = ArcGISElement{ localResult };
    }
    
    return localLocalResult;
}

GameEngine::Security::ArcGISAuthenticationConfiguration ArcGISElement::GetValueAsArcGISAuthenticationConfiguration() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsArcGISAuthenticationConfiguration(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    auto localLocalResult = GameEngine::Security::ArcGISAuthenticationConfiguration{};
    
    if (localResult != nullptr)
    {
        localLocalResult = GameEngine::Security::ArcGISAuthenticationConfiguration{ localResult };
    }
    
    return localLocalResult;
}

GameEngine::Attributes::ArcGISAttribute ArcGISElement::GetValueAsArcGISAttribute() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsAttribute(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    auto localLocalResult = GameEngine::Attributes::ArcGISAttribute{};
    
    if (localResult != nullptr)
    {
        localLocalResult = GameEngine::Attributes::ArcGISAttribute{ localResult };
    }
    
    return localLocalResult;
}

bool ArcGISElement::GetValueAsBool() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsBool(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

Unreal::ArcGISException ArcGISElement::GetValueAsError() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsError(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    auto localLocalResult = Unreal::ArcGISException{};
    
    if (localResult != nullptr)
    {
        localLocalResult = Unreal::ArcGISException{ localResult };
    }
    
    return localLocalResult;
}

float ArcGISElement::GetValueAsFloat32() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsFloat32(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

double ArcGISElement::GetValueAsFloat64() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsFloat64(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

int16_t ArcGISElement::GetValueAsInt16() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsInt16(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

int32_t ArcGISElement::GetValueAsInt32() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsInt32(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

int64_t ArcGISElement::GetValueAsInt64() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsInt64(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

int8_t ArcGISElement::GetValueAsInt8() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsInt8(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

FString ArcGISElement::GetValueAsString() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsString(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    auto localLocalResult = FString{};
    
    if (localResult != nullptr)
    {
        localLocalResult = FString{ RT_String_cStr(localResult, &errorHandler) };
        RT_String_destroy(localResult, &errorHandler);
    }
    
    return localLocalResult;
}

uint16_t ArcGISElement::GetValueAsUInt16() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsUInt16(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

uint32_t ArcGISElement::GetValueAsUInt32() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsUInt32(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

uint64_t ArcGISElement::GetValueAsUInt64() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsUInt64(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

uint8_t ArcGISElement::GetValueAsUInt8() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsUInt8(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

GameEngine::Attributes::ArcGISVisualizationAttribute ArcGISElement::GetValueAsArcGISVisualizationAttribute() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsVisualizationAttribute(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    auto localLocalResult = GameEngine::Attributes::ArcGISVisualizationAttribute{};
    
    if (localResult != nullptr)
    {
        localLocalResult = GameEngine::Attributes::ArcGISVisualizationAttribute{ localResult };
    }
    
    return localLocalResult;
}

GameEngine::Attributes::ArcGISVisualizationAttributeDescription ArcGISElement::GetValueAsArcGISVisualizationAttributeDescription() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_getValueAsVisualizationAttributeDescription(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    auto localLocalResult = GameEngine::Attributes::ArcGISVisualizationAttributeDescription{};
    
    if (localResult != nullptr)
    {
        localLocalResult = GameEngine::Attributes::ArcGISVisualizationAttributeDescription{ localResult };
    }
    
    return localLocalResult;
}

bool ArcGISElement::IsEmpty() const
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    auto localResult = RT_Element_isEmpty(reinterpret_cast<RT_ElementHandle>(GetHandle()), &errorHandler);
    
    return localResult;
}

void ArcGISElement::SetValueFromArcGISAuthenticationConfiguration(const GameEngine::Security::ArcGISAuthenticationConfiguration& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localValue = static_cast<const RT_ArcGISAuthenticationConfigurationHandle>(value.GetHandle());
    
    RT_Element_setValueFromArcGISAuthenticationConfiguration(reinterpret_cast<RT_ElementHandle>(GetHandle()), localValue, &errorHandler);
}

void ArcGISElement::SetValueFromArcGISAttribute(const GameEngine::Attributes::ArcGISAttribute& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localValue = static_cast<const RT_AttributeHandle>(value.GetHandle());
    
    RT_Element_setValueFromAttribute(reinterpret_cast<RT_ElementHandle>(GetHandle()), localValue, &errorHandler);
}

void ArcGISElement::SetValueFromBool(bool value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromBool(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromFloat32(float value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromFloat32(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromFloat64(double value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromFloat64(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromInt16(int16_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromInt16(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromInt32(int32_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromInt32(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromInt64(int64_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromInt64(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromInt8(int8_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromInt8(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromString(const FString& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromString(reinterpret_cast<RT_ElementHandle>(GetHandle()), TCHAR_TO_ANSI(*value), &errorHandler);
}

void ArcGISElement::SetValueFromUInt16(uint16_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromUInt16(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromUInt32(uint32_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromUInt32(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromUInt64(uint64_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromUInt64(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromUInt8(uint8_t value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    RT_Element_setValueFromUInt8(reinterpret_cast<RT_ElementHandle>(GetHandle()), value, &errorHandler);
}

void ArcGISElement::SetValueFromArcGISVisualizationAttribute(const GameEngine::Attributes::ArcGISVisualizationAttribute& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localValue = static_cast<const RT_VisualizationAttributeHandle>(value.GetHandle());
    
    RT_Element_setValueFromVisualizationAttribute(reinterpret_cast<RT_ElementHandle>(GetHandle()), localValue, &errorHandler);
}

void ArcGISElement::SetValueFromArcGISVisualizationAttributeDescription(const GameEngine::Attributes::ArcGISVisualizationAttributeDescription& value)
{
    auto errorHandler = Unreal::CreateErrorHandler();
    
    const auto localValue = static_cast<const RT_VisualizationAttributeDescriptionHandle>(value.GetHandle());
    
    RT_Element_setValueFromVisualizationAttributeDescription(reinterpret_cast<RT_ElementHandle>(GetHandle()), localValue, &errorHandler);
}

ArcGISElement::ArcGISElement(void* handle) :
    m_handle{ handle }
{
}

ArcGISElement::ArcGISElement(ArcGISElement&& other) noexcept :
    m_handle{ other.m_handle }
{
    other.m_handle = nullptr;
}

ArcGISElement::~ArcGISElement()
{
    if (m_handle)
    {
        auto errorHandler = Unreal::CreateErrorHandler();
        
        RT_Element_destroy(reinterpret_cast<RT_ElementHandle>(m_handle), &errorHandler);
    }
}

ArcGISElement& ArcGISElement::operator=(ArcGISElement&& other) noexcept
{
    if (this != &other)
    {
        using std::swap;
        
        swap(m_handle, other.m_handle);
    }
    
    return *this;
}

ArcGISElement::operator bool() const noexcept
{
    return m_handle != nullptr;
}

void* ArcGISElement::GetHandle() const
{
    return m_handle;
}
void ArcGISElement::SetHandle(void* handle)
{
    m_handle = handle;
}
} // namespace Standard
} // namespace Esri
