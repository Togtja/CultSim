#pragma once
#include <cstdint>
#include <string>
namespace cs
{
enum ETag : uint64_t
{
    TAG_None      = 0,
    TAG_Food      = 1 << 1,
    TAG_Drink     = 1 << 2,
    TAG_Sleep     = 1 << 3,
    TAG_Joy       = 1 << 4,
    TAG_Location  = 1 << 5,
    TAG_Find      = 1 << 6,
    TAG_Vision    = 1 << 7,
    TAG_Avoidable = 1 << 8,
    TAG_Reproduce = 1 << 9,
    TAG_Human     = 1 << 10,
    TAG_Tag       = 1 << 11
};

inline std::string tag_to_string(ETag tag)
{
    switch (uint64_t(tag))
    {
        case 0: return "None";
        case 2: return "Food"; 
        case 4: return "Drink"; 
        case 8: return "Sleep";
        case 16: return "Joy";
        case 32: return "Location";
        case 64: return "Find";
        case 128: return "Vision";
        case 256: return "Avoidable";
        case 512: return "Reproduce";
        case 1024: return "Human";
        case 2048: return "Tag";
        default: return "Error";
    }
}
} // namespace cs
