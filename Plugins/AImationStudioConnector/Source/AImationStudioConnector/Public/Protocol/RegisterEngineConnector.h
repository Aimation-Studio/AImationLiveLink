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
    FString EngineName = "Unreal Engine";
};