
#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include <JsonObjectConverter.h>
#include <Templates/UnrealTypeTraits.h>
#include <Serialization/JsonReader.h>
#include <Dom/JsonObject.h>
#include <Templates/Function.h>
#include "JsonUtilities.h"

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

namespace detail
{
    template< typename T, typename Arg >
    using MemberFunction = void(T ::*)(Arg);
};

class PacketHandlerMgr
{
public:
    PacketHandlerMgr() = default;

    template <typename Packet, typename U>
    typename TEnableIf<std::is_same_v<decltype(Packet::HandlerID), uint32>>::Type
        RegisterPacketHandler(U* self, detail::MemberFunction<U, Packet const&> callback)
    {
        uint32 handlerId = Packet{}.HandlerID;
        auto handler = [self, callback, _handlerId = handlerId](FString const& msg)
        {
            try
            {
                TSharedPtr<FJsonObject> JsonObject;
                TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(std::move(msg));

                Packet deserialized{};
                if (!FJsonObjectConverter::JsonObjectStringToUStruct(msg, &deserialized, 0, 0))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize packet %d"), _handlerId);
                    return false;
                }

                (self->*callback)(*reinterpret_cast<Packet const*>(&deserialized));
                return true;
            }
            catch (std::exception& e)
            {
                UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize packet %d: %hs"), _handlerId, e.what());
                return false;
            }
        };

        if (m_handlers.Contains(handlerId))
        {
            checkf(false, TEXT("Packet handler for %d already registered"), handlerId);
        }

        m_handlers.Add(handlerId, std::move(handler));
    }

protected:
    TMap<uint32, TFunction<bool(FString const&)>> m_handlers;
};
