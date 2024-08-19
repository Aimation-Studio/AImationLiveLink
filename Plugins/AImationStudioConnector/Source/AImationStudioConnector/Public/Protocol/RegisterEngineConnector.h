// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "RegisterEngineConnector.generated.h"

UENUM()
enum class PoseType : uint32
{
    LocalCoordsPose = 0,
    WorldCoordsPose = 1
};

USTRUCT()
struct FRegisterEngineConnectorPacket
{
    GENERATED_BODY()

        UPROPERTY()
        uint32 HandlerID = 14;

    UPROPERTY()
        FString ClientName = "Unreal Engine";

    UPROPERTY()
        uint32 RequestedPoseType = 0; //TODO: fix serializing enums between UE and AImation, then replace this with PoseType
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
        ResponseCode Response = ResponseCode::Success;

    UPROPERTY()
        PoseType AimationPose = PoseType::LocalCoordsPose;
};

USTRUCT()
struct FUnregisterEngineConnectorPacket
{
    GENERATED_BODY()
public:

    UPROPERTY()
        uint32 HandlerID = 10000000;

    UPROPERTY()
        FString EngineName = "Unreal Engine";
};

USTRUCT()
struct FUnregisterEngineConnectorResponsePacket
{
    GENERATED_BODY()
public:

    UPROPERTY()
        uint32 HandlerID = 160000;
};
