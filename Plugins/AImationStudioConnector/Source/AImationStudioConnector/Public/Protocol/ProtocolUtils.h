
#pragma once

#include "BinaryBlobData.hpp"
#include "Containers/Array.h"
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "JsonObjectConverter.h"
#include "JsonUtilities.h"
#include "Serialization/JsonReader.h"
#include "Templates/Function.h"
#include "Templates/UnrealTypeTraits.h"

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
    using AppendedBinaryStore = TArray<aimation::BinaryBlobData>;

    template< typename T, typename Packet >
    using MemberFunction = void(T ::*)(const Packet&, const AppendedBinaryStore&);
};

class PacketHandlerMgr
{
public:
    PacketHandlerMgr() = default;

    template <typename Packet, typename U>
    typename TEnableIf<std::is_same_v<decltype(Packet::HandlerID), uint32>>::Type
        RegisterPacketHandler(U* self, detail::MemberFunction<U, Packet> callback)
    {
        uint32 handlerId = Packet{}.HandlerID;
        auto handler = [self, callback, _handlerId = handlerId](FString const& msg, const detail::AppendedBinaryStore& appendedBinaryData)
        {
            try
            {
                Packet deserialized{};
                if (!FJsonObjectConverter::JsonObjectStringToUStruct(msg, &deserialized, 0, CPF_SkipSerialization))
                {
                    UE_LOG(LogTemp, Warning, TEXT("Failed to deserialize packet %d"), _handlerId);
                    return false;
                }

                // we pass in the binary data as argument because it is hard to deserialize those fields in a generic way
                // because unreal does not support reflecting on std types and the binary data is usually an std type of some kind serialized to binary
                (self->*callback)(*reinterpret_cast<Packet const*>(&deserialized), appendedBinaryData);
                return true;
            }
            catch (std::exception& e) // we're no longer using nlohmann here, this exception will never be caught TODO
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
    using PacketHandler = TFunction<bool(FString const&, const detail::AppendedBinaryStore&)>;
    TMap<uint32, PacketHandler> m_handlers;
};
