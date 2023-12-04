// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "Protocol/ProtocolUtils.h"
#include <Containers/UnrealString.h>

struct FRegisterEngineConnectorResponsePacket;

class AIMATIONSTUDIOCONNECTOR_API UAimationWebSocket : public PacketHandlerMgr
{
public:
    UAimationWebSocket();
    ~UAimationWebSocket();

    bool Connect(const FString& InUrl);
    bool Disconnect();
    inline bool IsConnected() const { return WebSocket.IsValid() && WebSocket->IsConnected(); }
    auto& OnBinaryMessageDelegate() { return WebSocket->OnBinaryMessage(); }
    auto & OnConnected() { return WebSocket->OnConnected(); }
    auto & OnConnectionError() { return WebSocket->OnConnectionError(); }
    auto & OnClosed() { return WebSocket->OnClosed(); }
    auto & OnMessage() { return WebSocket->OnMessage(); }

    template< typename T >
    FORCEINLINE void SendPacket(T& packet)
    {
        FString out{};
        AimationHelpers::PacketToString(packet, out, 0, 0, 0);
        auto registerEnginePkt = AimationHelpers::CreateAimationPacket(out);
        WebSocket->Send(registerEnginePkt.GetData(), registerEnginePkt.Num(), true);
    }

private:
    TSharedPtr<IWebSocket> WebSocket;
    std::vector<uint8> m_receiveBuffer;

protected:
    void OnBinaryMessage(const void* InData, SIZE_T InSize, bool isLastFragment);
};
