#pragma once
#include "memory.h"

#include <vector>
namespace cs::memory
{
struct Container
{
public:
    ETag memory_tags{};

    std::vector<IMemory> memory_storage{};

    Container() = default;
    Container(ETag tags) : memory_tags(tags){};
    Container(const Container&) = delete;
    Container& operator=(const Container&) = delete;

    Container(Container&&) = default;
    Container& operator=(Container&&) = default;

};
} // namespace ::cs::memory