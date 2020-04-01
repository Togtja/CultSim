#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace cs
{
enum ETag : uint64_t
{
    TAG_None      = 0ULL,
    TAG_Food      = 1ULL << 1,
    TAG_Drink     = 1ULL << 2,
    TAG_Sleep     = 1ULL << 3,
    TAG_Joy       = 1ULL << 4,
    TAG_Location  = 1ULL << 5,
    TAG_Find      = 1ULL << 6,
    TAG_Vision    = 1ULL << 7,
    TAG_Avoidable = 1ULL << 8,
    TAG_Reproduce = 1ULL << 9,
    TAG_Creature  = 1ULL << 10,
    TAG_Tag       = 1ULL << 11,
    TAG_Gather    = 1ULL << 12,
    TAG_Delete    = 1ULL << 13,
    TAG_Reserved  = 1ULL << 14,
    TAG_Carnivore = 1ULL << 15,
    TAG_Herbivore = 1ULL << 16,
    TAG_Omnivore  = 1ULL << 17,
    TAG_Meat      = 1ULL << 18,
    TAG_Veggie    = 1ULL << 19,
    TAG_Inventory = 1ULL << 20,
    TAG_Consume   = 1ULL << 21
};

inline std::string tag_to_string(ETag tag)
{
    std::string result{};
    std::vector<std::string> tag_names{"None",   "Food",   "Drink",     "Sleep",     "Joy",       "Location",
                                       "Find",   "Vision", "Avoidable", "Reproduce", "Creature",  "Tag",
                                       "Gather", "Delete", "Reserved",  "Carnivore", "Herbivore", "Omnivore",
                                       "Meat",   "Veggie", "Inventory", "Consume"};

    for (unsigned i = 0u; i < tag_names.size(); i++)
    {
        if (tag & 1 << i)
        {
            result += " " + tag_names[i];
        }
    }

    return result;
}
} // namespace cs
