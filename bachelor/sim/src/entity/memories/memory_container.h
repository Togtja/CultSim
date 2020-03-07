#pragma once

#include "memory.h"
#include "entity/components/tags.h"

#include <memory>
#include <vector>

namespace cs
{
struct MemoryContainer
{
    ETag memory_tag{};

    std::vector<std::unique_ptr<IMemory>> memory_container{};

    MemoryContainer() = default;
    MemoryContainer(ETag tags) : memory_tag(tags){};
    MemoryContainer(const MemoryContainer&) = delete;
    MemoryContainer& operator=(const MemoryContainer&) = delete;

    MemoryContainer(MemoryContainer&&) = default;
    MemoryContainer& operator=(MemoryContainer&&) = default;
};
} // namespace cs
