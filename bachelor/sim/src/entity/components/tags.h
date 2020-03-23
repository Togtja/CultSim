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
    TAG_Tag       = 1 << 11,
    TAG_Gather    = 1 << 12,
    TAG_Delete    = 1 << 13,
    TAG_Reserved  = 1 << 14
};

inline constexpr std::string_view tag_to_string(ETag tag)
{
    switch (uint64_t(tag))
    {
        case 0: return "None";
        case 1 << 1: return "Food";
        case 1 << 2: return "Drink";
        case 1 << 3: return "Sleep";
        case 1 << 4: return "Joy";
        case 1 << 5: return "Location";
        case 1 << 6: return "Find";
        case 1 << 7: return "Vision";
        case 1 << 8: return "Avoidable";
        case 1 << 9: return "Reproduce";
        case 1 << 10: return "Human";
        case 1 << 11: return "Tag";
        case 1 << 12: return "Gather";
        case 1 << 13: return "Delete";
        case 1 << 14: return "Reserved";
        default: return "Error";
    }
}
} // namespace cs
