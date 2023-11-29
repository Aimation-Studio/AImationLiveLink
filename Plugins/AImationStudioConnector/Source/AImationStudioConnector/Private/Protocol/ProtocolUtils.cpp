
#include "Protocol/ProtocolUtils.h"

#include <Containers/StringConv.h>
#include <Containers/UnrealString.h>
#include <Serialization/MemoryWriter.h>
#include <Policies/CondensedJsonPrintPolicy.h>
#include <Serialization/JsonSerializer.h>
#include <Serialization/JsonWriter.h>

TArray<uint8> AimationHelpers::CreateAimationPacket(FString& json, uint32 headerType /*= 6u*/)
{
    // Convert MagicNumber to UTF-8
    int32 MagicNumberUtf8Length = FTCHARToUTF8_Convert::ConvertedLength(TEXT("AiMation"), 8);
    TArray<uint8> MagicNumberBuffer;
    MagicNumberBuffer.SetNumUninitialized(MagicNumberUtf8Length);
    FTCHARToUTF8_Convert::Convert(reinterpret_cast<UTF8CHAR*>(MagicNumberBuffer.GetData()), MagicNumberBuffer.Num(), TEXT("AiMation"), 8);

    // Convert FString to UTF-8
    int32 Utf8Length = FTCHARToUTF8_Convert::ConvertedLength(*json, json.Len());
    TArray<uint8> JsonBuffer;
    JsonBuffer.SetNumUninitialized(Utf8Length);
    FTCHARToUTF8_Convert::Convert(reinterpret_cast<UTF8CHAR*>(JsonBuffer.GetData()), JsonBuffer.Num(), *json, json.Len());

    uint32 PacketSize = MagicNumberUtf8Length + sizeof(uint32) * 4 + Utf8Length; // MagicNumber + BlockSize + HeaderType + DataStartOffset + DataSize + JsonData

    TArray<uint8> packet;
    packet.SetNumZeroed(PacketSize);

    FMemoryWriter Writer(packet, false);

    Writer.Serialize(MagicNumberBuffer.GetData(), MagicNumberBuffer.Num());

    Writer.Seek(16);
    // Set block size to 1
    uint32 BlockSize = 1;
    Writer << BlockSize;

    Writer.Seek(24);
    // Set default data type to 6 (header type)
    uint32 HeaderType = 6;
    Writer << HeaderType;

    Writer.Seek(28);
    // Set data start offset
    uint32 DataStartOffset = 36;
    Writer << DataStartOffset;

    Writer.Seek(32);
    // Set data size
    uint32 EncodedLength = json.Len();
    Writer << EncodedLength;

    Writer.Seek(36);
    // Append the JSON data to the packet
    Writer.Serialize(JsonBuffer.GetData(), JsonBuffer.Num());
    return packet;
}

bool AimationHelpers::PacketToStringImpl(const UStruct* StructDefinition, const void* Struct, FString& OutJsonString, int64 CheckFlags /*= 0*/, int64 SkipFlags /*= 0*/, int32 Indent /*= 0*/)
{
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    //UStructToJsonObject(StructDefinition, Struct, JsonObject, CheckFlags, SkipFlags, nullptr);

    if (SkipFlags == 0)
    {
        // If we have no specified skip flags, skip deprecated, transient and skip serialization by default when writing
        SkipFlags |= CPF_Deprecated | CPF_Transient;
    }

    if (StructDefinition == FJsonObjectWrapper::StaticStruct())
    {
        // Just copy it into the object
        const FJsonObjectWrapper* ProxyObject = (const FJsonObjectWrapper*)Struct;

        if (ProxyObject->JsonObject.IsValid())
        {
            JsonObject->Values = ProxyObject->JsonObject->Values;
        }
        return true;
    }

    for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
    {
        FProperty* Property = *It;

        // Check to see if we should ignore this property
        if (CheckFlags != 0 && !Property->HasAnyPropertyFlags(CheckFlags))
        {
            continue;
        }
        if (Property->HasAnyPropertyFlags(SkipFlags))
        {
            continue;
        }

        FString VariableName = Property->GetAuthoredName();
        const void* Value = Property->ContainerPtrToValuePtr<uint8>(Struct);

        // convert the property to a FJsonValue
        TSharedPtr<FJsonValue> JsonValue = FJsonObjectConverter::UPropertyToJsonValue(Property, Value, CheckFlags, SkipFlags, nullptr);
        if (!JsonValue.IsValid())
        {
            FFieldClass* PropClass = Property->GetClass();
            UE_LOG(LogTemp, Error, TEXT("UStructToJsonObject - Unhandled property type '%s': %s"), *PropClass->GetName(), *Property->GetPathName());
            return false;
        }

        // set the value on the output object
        JsonObject->Values.Add(VariableName, JsonValue);
    }

    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>> > JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutJsonString, Indent);
    bool bSuccess = FJsonSerializer::Serialize(JsonObject, JsonWriter);
    JsonWriter->Close();
    return bSuccess;
}
