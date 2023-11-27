
#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include <JsonObjectConverter.h>

struct AimationHelpers
{
    /// @brief Takes a json string and creates a binary packet with a proper header that AImation Studio accepts
    /// @param json The json string to send to AImation Studio
    /// @param headerType The header type to use, defaults to 6 (Json Type)
    /// @return The binary packet to send to AImation Studio
    /// @note Each packet requires at least HandlerID field, otherwise AImation Studio will ignore it or even might disconnect the client
    static TArray<uint8> CreateAimationPacket(FString& json, uint32 headerType = 6u);

    static bool PacketToStringImpl(const UStruct* StructDefinition, const void* Struct, FString& OutJsonString, int64 CheckFlags = 0, int64 SkipFlags = 0, int32 Indent = 0);

    template<typename InStructType>
    static bool PacketToString(const InStructType& InStruct, FString& OutJsonString, int64 CheckFlags = 0, int64 SkipFlags = 0, int32 Indent = 0)
    {
        return PacketToStringImpl(InStructType::StaticStruct(), &InStruct, OutJsonString, CheckFlags, SkipFlags, Indent);
    }
};

