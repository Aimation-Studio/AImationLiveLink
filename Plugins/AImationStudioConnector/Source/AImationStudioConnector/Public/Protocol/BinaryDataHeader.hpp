
#pragma once

#include "CoreMinimal.h"
#include "Serialization/Archive.h"

struct FDataHeaderEntry
{
    uint32 Type = 0;
    uint32 Position = 0;
    uint32 Size = 0;

    friend FArchive& operator<<(FArchive& Ar, FDataHeaderEntry& Entry)
    {
        Ar << Entry.Type;
        Ar << Entry.Position;
        Ar << Entry.Size;
        return Ar;
    }
};
