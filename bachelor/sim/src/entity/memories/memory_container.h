#pragma once

#include "memory.h"
#include <entity/components/tags.h>

namespace cs
{

struct MemoryContainer
{
    ETag memory_tag{};

    std::vector<std::unique_ptr<IMemory>> memory_Container{};
};
} // namespace cs
