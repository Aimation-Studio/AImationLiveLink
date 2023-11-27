
#include "AimationWebSocket.h"
#include "AImationStudioConnector.h"
#include "Utils2.h"

#include "Protocol/RegisterEngineConnector.h"

UAimationWebSocket::UAimationWebSocket() : WebSocket(nullptr)
{
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

void UAimationWebSocket::SendBinaryData(const TArray<uint8>& InData)
{

}

void UAimationWebSocket::OnConnected()
{
    UE_LOG(LogTemp, Log, TEXT("AimationWebSocket connected"));

    FRegisterEngineConnectorPacket Packet;
    Packet.EngineName = "Unreal Engine";

    // TODO: generics
    FString out{};
    AimationHelpers::PacketToString( Packet, out, 0, 0, 0 );
    auto registerEnginePkt = AimationHelpers::CreateAimationPacket(out);
    WebSocket->Send(registerEnginePkt.GetData(), registerEnginePkt.Num(), true);
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
    UE_LOG(LogTemp, Log, TEXT("AimationWebSocket received binary message"));
}
