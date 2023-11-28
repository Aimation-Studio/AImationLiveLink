
#include "AimationWebSocket.h"
#include "AImationStudioConnector.h"
#include "Utils2.h"

#include "Protocol/RegisterEngineConnector.h"

UAimationWebSocket::UAimationWebSocket() : WebSocket(nullptr)
{
    // preallocate a 64mb receiving buffer
    m_receiveBuffer.Reserve(64 * 1024 * 1024);
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

///#include "ThirdParty/nlohmann_json/json.hpp"
#include "ThirdParty/nlohmann_json/json.hpp"

void UAimationWebSocket::OnBinaryMessage(const void* InData, SIZE_T InSize, bool isLastFragment)
{
    // Convert the binary data to a TArray<uint8>
    TArray<uint8> ReceivedBytes(reinterpret_cast<const uint8*>(InData), InSize);

    // Convert received bytes from_msgpack using nlohmann::json
    nlohmann::json j = nlohmann::json::from_msgpack(ReceivedBytes.GetData());
    auto dumped = j.dump();
    UE_LOG(LogTemp, Log, TEXT("AimationWebSocket received message: %s"), *FString(dumped.c_str()));
    //// Deserialize the MessagePack object using RPCLIB_MSGPACK
    //clmdep_msgpack::zone z;
    //clmdep_msgpack::object_handle oh = clmdep_msgpack::unpack(ReceivedBytes.GetData(), ReceivedBytes.Num(), &z);

    //// Assuming the deserialization was successful
    //const clmdep_msgpack::object& msgpackObject = oh.get();

    //// Convert the MessagePack object to an Unreal JsonObject
    //FJsonObject JsonObject;

    //// Use the appropriate serialization function based on the content of msgpackObject
    //msgpackObject.msgpack_object(&JsonObject, z);
}
