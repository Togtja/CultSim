#pragma once

namespace cs
{
class IMemory
{
public:
    ETag tags{};
    IMemory(ETag tag) : tags(tag){};
};
} // namespace cs