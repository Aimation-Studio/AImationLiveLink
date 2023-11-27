// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "JsonUtilities.h" // Include for JSON serialization
#include <Containers/UnrealString.h>
#include <Delegates/DelegateCombinations.h>
//#include "AimationWebSocket.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBinaryMessageReceived, const FString&, JsonData);

//UCLASS()
class AIMATIONSTUDIOCONNECTOR_API UAimationWebSocket/* : public UObject*/
{
    //GENERATED_BODY()
public:
    UAimationWebSocket();
    ~UAimationWebSocket();

    void Connect(const FString& InUrl);
    void Disconnect();
    void SendBinaryData(const TArray<uint8>& InData);
    void OnConnected();
    void OnConnectionError(const FString& Error);
    void OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean);
    void OnMessage(const FString& Message);
    inline bool IsConnected() const { return WebSocket.IsValid() && WebSocket->IsConnected(); }
    //FOnBinaryMessageReceived OnBinaryMessageReceived;

private:
    TSharedPtr<IWebSocket> WebSocket;

    void OnBinaryMessage(const void* InData, SIZE_T InSize, bool isLastFragment);
};
