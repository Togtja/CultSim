#pragma once
#include "memory.h"

#include <vector>
namespace cs::memory
{
struct Container
{
public:
    ETag memory_tags{};
    std::vector<std::unique_ptr<IMemory>> memory_storage{};

    Container() = default;
    Container(ETag tags) : memory_tags(tags){};

    Container(const Container& other) : memory_tags(other.memory_tags)
    {
        for (const auto& ptr : other.memory_storage)
        {
            memory_storage.emplace_back(ptr->clone());
        }
    }

    Container& operator=(const Container& other)
    {
        /** Protect against self-copy */
        if (this == &other)
        {
            return *this;
        }

        /** Clean up existing members */
        memory_storage.clear();
        memory_tags = ETag::TAG_None;

        /** Copy members from the other */
        memory_tags = other.memory_tags;
        for (const auto& ptr : other.memory_storage)
        {
            memory_storage.emplace_back(ptr->clone());
        }

        return *this;
    }

    Container(Container&&) = default;
    Container& operator=(Container&&) = default;
};
} // namespace cs::memory
