
#include "AimationWebSocket.h"
#include "AImationStudioConnector.h"
#include "Protocol/ProtocolUtils.h"

#include "ThirdParty/nlohmann_json/json.hpp"
#include "Protocol/BinaryDataHeader.hpp"
#include "Protocol/BinaryBlobData.hpp"
#include "Algo/Find.h"

UAimationWebSocket::UAimationWebSocket() : WebSocket(nullptr)
{
}

UAimationWebSocket::~UAimationWebSocket()
{
}

bool UAimationWebSocket::Connect(const FString& InUrl)
{
    if (!WebSocket.IsValid())
    {
        WebSocket = FWebSocketsModule::Get().CreateWebSocket(InUrl);

        if (!WebSocket.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("AimationWebSocket failed to create socket for url"));
            return false;
        }
    }

    m_receiveBuffer.reserve(64 * 1024);
    WebSocket->SetTextMessageMemoryLimit(64 * 1024);
    WebSocket->OnBinaryMessage().AddRaw(this, &UAimationWebSocket::OnBinaryMessage);
    WebSocket->Connect();
    return true;
}

bool UAimationWebSocket::Disconnect()
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Close();
        WebSocket = nullptr;
        return true;
    }

    return false;
}

// Constants
constexpr uint64 c_magic_number_aimation_header = 0x6E6F6974614D6941ull; // "AiMation" in little-endian
constexpr uint8 c_serialization_abi_version = 0; // Replace 1 with AV_base or actual version
enum PacketDataType
{
    None = 0,
    Json = 6,
    Vector3 = 3,
    String = 5,
    Float = 7,
    Quaternion = 11,
};

size_t GetSizeForType(const PacketDataType type)
{
    switch (type)
    {
        case PacketDataType::Json:
            return sizeof(nlohmann::json);
        case PacketDataType::Vector3:
            return 3.f * sizeof(float);
        case PacketDataType::String:
            return sizeof(std::string);
        case PacketDataType::Float:
            return sizeof(float);
        case PacketDataType::Quaternion:
            return 4.f * sizeof(float);
        default:
            return 0;
    }
}

void UAimationWebSocket::DecomposeBuffer()
{
    // convert std::vector buffer to TArray
    TArray<uint8> ueBuffer;
    ueBuffer.SetNumUninitialized(m_receiveBuffer.size());

    for (int32 i = 0; i < m_receiveBuffer.size(); ++i)
    {
        ueBuffer[i] = m_receiveBuffer[i];
    }
    m_receiveBuffer.clear();

    FMemoryReader MemoryReader(ueBuffer, true);

    // Read the header
    std::array<uint64, 3> Forehead;
    MemoryReader.Serialize(Forehead.data(), sizeof(uint64) * 3);

    if (Forehead[0] != c_magic_number_aimation_header)
    {
        UE_LOG(LogTemp, Error, TEXT("Binary read context data is damaged."));
    }

    uint8 ReadAbiVersion = (Forehead[1] & 0xFF00000000000000ull) >> 56;
    if (ReadAbiVersion > c_serialization_abi_version)
    {
        UE_LOG(LogTemp, Error, TEXT("Binary read context cannot deserialize because of version mismatch."));
        throw std::runtime_error("Binary read context cannot deserialize because of version mismatch.");
    }

    TArray<FDataHeaderEntry> Headers;
    Headers.SetNum(Forehead[2]);
    for (auto& Entry : Headers)
        MemoryReader << Entry;

    TArray<bool> Used;
    Used.Init(false, Forehead[2]);

    // Log the header information and data entries
    UE_LOG(LogTemp, Verbose, TEXT("Magic Number: %llu"), Forehead[0]);
    UE_LOG(LogTemp, Verbose, TEXT("Reserved: %llu"), Forehead[1]);
    UE_LOG(LogTemp, Verbose, TEXT("Blocks Count: %llu"), Forehead[2]);

    // This will store our json packet coming from aimation, as such we will parse it with nlohmann::json
    // and nlohmann has no support for TArray<uint8> so we will use std::vector<uint8> instead
    std::vector<uint8> jsonPacketData{ };

    // rest of the packets can be either binary or json, for now we assume it's binary
    TArray< aimation::BinaryBlobData > binaryPacketsData{ };

    for (int32 i = 0; i < Headers.Num(); ++i)
    {
        if (Used[i])
            continue;

        if (Headers[i].Type == PacketDataType::Json)
        {
            jsonPacketData.resize(Headers[i].Size );
            MemoryReader.Seek(Headers[i].Position);
            MemoryReader.Serialize(jsonPacketData.data(), Headers[i].Size);
            Used[i] = true;
        }
        // everything else is appended binary data of some kind, depends on the packet, note the fields that have property SkipSerialization
        else
        {
            aimation::BinaryBlobData blob{};
            blob.header = Headers[i];

            auto sizeForType = GetSizeForType(static_cast<PacketDataType>(Headers[i].Type));
            blob.data.SetNumUninitialized(Headers[i].Size * sizeForType);
            MemoryReader.Seek(Headers[i].Position);
            MemoryReader.Serialize(blob.data.GetData(), Headers[i].Size * sizeForType);
            binaryPacketsData.Add(MoveTemp(blob));
            Used[i] = true;
        }
    }

    // parse the json packet
    if (jsonPacketData.size() > 0)
    {
        constexpr bool G_ALLOW_JSON_EXCEPTIONS{ true };
        try
        {
            // first we parse the json packet to get the handler id, this also performs a validation in a way
            nlohmann::json j = nlohmann::json::parse(jsonPacketData, nullptr, G_ALLOW_JSON_EXCEPTIONS);
            checkf(!j.is_discarded(), TEXT("AimationWebSocket received failed to turn msgpack buffer into json object"));

            UE_LOG(LogTemp, Verbose, TEXT("AimationWebSocket received json packet: %s"), UTF8_TO_TCHAR(j.dump().c_str()));
            if (j.contains("HandlerID"))
            {
                uint32 handlerID = j["HandlerID"];
                if (m_handlers.Contains(handlerID))
                {
                    // create an unreal string from jsonPacketData
                    //FString uJsonString = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(jsonPacketData.data())));
                    std::string cppString(jsonPacketData.begin(), jsonPacketData.end());

                    FString uJsonString = FString(cppString.c_str());
                    m_handlers[handlerID](uJsonString, binaryPacketsData);
                }
            }
        }
        catch (std::exception& e)
        {
            auto exceptionReason = e.what();
            UE_LOG(LogTemp, Error, TEXT("AimationWebSocket received failed to turn msgpack buffer into json object: %s"), UTF8_TO_TCHAR(exceptionReason));
            return;
        }
    }
}

void UAimationWebSocket::OnBinaryMessage(const void* InData, SIZE_T InSize, bool isLastFragment)
{
    // turn incoming data into vector<uint8>
    const uint8* data = reinterpret_cast<const uint8*>(InData);
    std::vector<uint8> dataVector(data, data + InSize);
    m_receiveBuffer.insert(m_receiveBuffer.end(), dataVector.begin(), dataVector.end());

    if (isLastFragment)
    {
        DecomposeBuffer();
        // clear the array without resizing the buffer, as resizing can change the layout of message pack and crash
        m_receiveBuffer.clear();
    }
}

