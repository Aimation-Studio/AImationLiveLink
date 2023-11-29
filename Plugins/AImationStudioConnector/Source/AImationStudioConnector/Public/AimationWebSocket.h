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
    //GENERATED_BODY()
public:
    UAimationWebSocket();
    ~UAimationWebSocket();

    void Connect(const FString& InUrl);
    void Disconnect();
    void OnConnected();
    void OnConnectionError(const FString& Error);
    void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
    void OnMessage(const FString& Message);
    inline bool IsConnected() const { return WebSocket.IsValid() && WebSocket->IsConnected(); }
    //FOnBinaryMessageReceived OnBinaryMessageReceived;

    template< typename T >
    inline void SendPacket(T& packet);
private:
    TSharedPtr<IWebSocket> WebSocket;
    TArray<uint8> m_receiveBuffer;

    void OnBinaryMessage(const void* InData, SIZE_T InSize, bool isLastFragment);
protected:
    void OnRegisterEngineConnectorPacket( const FRegisterEngineConnectorResponsePacket& packet );
};
