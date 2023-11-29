
#include "AimationWebSocket.h"
#include "AImationStudioConnector.h"
#include "Protocol/ProtocolUtils.h"

#include "Protocol/RegisterEngineConnector.h"
#include "ThirdParty/nlohmann_json/json.hpp"

UAimationWebSocket::UAimationWebSocket() : WebSocket(nullptr)
{
    // preallocate a 64mb receiving buffer
    m_receiveBuffer.Reserve(64 * 1024 * 1024);

    RegisterPacketHandler< FRegisterEngineConnectorResponsePacket >(this, &UAimationWebSocket::OnRegisterEngineConnectorPacket);
}

UAimationWebSocket::~UAimationWebSocket()
{
}

void UAimationWebSocket::Connect(const FString& InUrl)
{
    WebSocket = FWebSocketsModule::Get().CreateWebSocket(InUrl);

    if (!WebSocket.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("AimationWebSocket failed to create socket for url"));
        return;
    }

    WebSocket->SetTextMessageMemoryLimit(64 * 1024 * 1024); // 64MB
    WebSocket->OnBinaryMessage().AddRaw(this, &UAimationWebSocket::OnBinaryMessage);
    WebSocket->OnConnected().AddRaw(this, &UAimationWebSocket::OnConnected);
    WebSocket->OnConnectionError().AddRaw(this, &UAimationWebSocket::OnConnectionError);
    WebSocket->OnClosed().AddRaw(this, &UAimationWebSocket::OnClosed);

    WebSocket->Connect();
}

void UAimationWebSocket::Disconnect()
{
    if (WebSocket.IsValid() && WebSocket->IsConnected())
    {
        WebSocket->Close();
        //WebSocket.Reset();
    }
}

template< typename T >
void UAimationWebSocket::SendPacket(T& packet)
{
    FString out{};
    AimationHelpers::PacketToString(packet, out, 0, 0, 0);
    auto registerEnginePkt = AimationHelpers::CreateAimationPacket(out);
    WebSocket->Send(registerEnginePkt.GetData(), registerEnginePkt.Num(), true);
}

void UAimationWebSocket::OnConnected()
{
    UE_LOG(LogTemp, Log, TEXT("AimationWebSocket connected"));

    FRegisterEngineConnectorPacket Packet;
    Packet.EngineName = "Unreal Engine hiha";

    SendPacket(Packet);
}

void UAimationWebSocket::OnConnectionError(const FString& Error)
{
    UE_LOG(LogTemp, Log, TEXT("AimationWebSocket connection error: %s"), *Error);
}

void UAimationWebSocket::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{

}

void UAimationWebSocket::OnMessage(const FString& Message)
{
    UE_LOG(LogTemp, Log, TEXT("AimationWebSocket received message: %s"), *Message);
}


void UAimationWebSocket::OnBinaryMessage(const void* InData, SIZE_T InSize, bool isLastFragment)
{
    TArray<uint8> ReceivedBytes(reinterpret_cast<const uint8*>(InData), InSize);
    m_receiveBuffer.Append(ReceivedBytes);

    if (isLastFragment)
    {
        constexpr bool G_ALLOW_JSON_EXCEPTIONS{ false };
        nlohmann::json j = nlohmann::json::from_msgpack(m_receiveBuffer.GetData(), true, G_ALLOW_JSON_EXCEPTIONS);
        checkf(!j.is_discarded(), TEXT("AimationWebSocket received failed to turn msgpack buffer into json object"));

        if (j.contains("HandlerID"))
        {
            uint32 handlerID = j["HandlerID"];
            if (m_receivedPacketHandlers.Contains(handlerID))
            {
                auto contents = j.dump();
                FString out{ contents.c_str() };

                m_receivedPacketHandlers[handlerID](out);
            }
        }

        // clear the array without resizing the buffer, as resizing can change the layout of message pack and crash
        m_receiveBuffer.Reset();
    }
}

void UAimationWebSocket::OnRegisterEngineConnectorPacket(const FRegisterEngineConnectorResponsePacket& packet)
{
    UE_LOG(LogTemp, Log, TEXT("AimationWebSocket received FRegisterEngineConnectorResponsePacket with status code: %d"), packet.Code);
}
