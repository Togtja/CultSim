#pragma once
#include <cstdint>
#include <string>
#include <vector>

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

inline std::string tag_to_string(ETag tag)
{
    std::string result{};
    std::vector<std::string> tag_names{"None",
                                       "Food",
                                       "Drink",
                                       "Sleep",
                                       "Joy",
                                       "Location",
                                       "Find",
                                       "Vision",
                                       "Avoidable",
                                       "Reproduce",
                                       "Human",
                                       "Tag",
                                       "Gather",
                                       "Delete",
                                       "Reserved"};
    for (int i = 0; i < tag_names.size(); i++)
    {
        if (tag & 1 << i)
        {
            result += " " + tag_names[i];
        }
    }

    return result;
}
} // namespace cs
