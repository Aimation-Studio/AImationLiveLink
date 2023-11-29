// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RegisterEngineConnector.generated.h"

USTRUCT()
struct FRegisterEngineConnectorPacket
{
    GENERATED_BODY()

        UPROPERTY()
        uint32 HandlerID = 14;

    UPROPERTY()
        FString ClientName = "Unreal Engine";
};

UENUM()
enum class ResponseCode : uint32
{
    Success = 0,
    ConnectorWithNameExists = 1,
    UnknownFailure = 2
};

USTRUCT()
struct FRegisterEngineConnectorResponsePacket
{
    GENERATED_BODY()
public:

    UPROPERTY()
        uint32 HandlerID = 15;

    UPROPERTY()
        ResponseCode Code = ResponseCode::Success;
};

USTRUCT()
struct FUnregisterEngineConnectorPacket
{
    GENERATED_BODY()
public:

    UPROPERTY()
        uint32 HandlerID = 15;

    UPROPERTY()
        FString EngineName = "Unreal Engine";
};

USTRUCT()
struct FUnregisterEngineConnectorResponsePacket
{
    GENERATED_BODY()
public:

    UPROPERTY()
        uint32 HandlerID = 16;
};
