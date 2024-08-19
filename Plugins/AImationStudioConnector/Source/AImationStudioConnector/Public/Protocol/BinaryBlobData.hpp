
#pragma once

#include "BinaryDataHeader.hpp"

#include <vector>

namespace aimation
{
    struct BinaryBlobData
    {
        FDataHeaderEntry header;
        TArray<uint8_t> data;
    };
}
