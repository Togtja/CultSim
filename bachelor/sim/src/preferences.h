#pragma once

#include "robin_hood.h"

#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include <glm/vec2.hpp>

namespace cs
{
/**
 * Defines a preference of some kind that can hold a value of supported preference types
 */
template<typename T>
struct Preference
{
    std::string name{};
    std::string description{};
    std::decay_t<T> value{};
};

class PreferenceManager
{
private:
    /** Window resolution */
    Preference<glm::ivec2> m_resolution{"Resolution", "The resolution of your game window", {1280, 720}};

    Preference<bool> m_fullscreen{"Fullscreen", "Should the window be fullscreen or windowed", false};

    /** Localization language */
    Preference<std::string> m_language{"Language", "Application language", "en"};

    /** General purpose storage for saved variables */
    robin_hood::unordered_map<std::string, std::string> m_saved_variables{};

public:
    PreferenceManager() = default;
    explicit PreferenceManager(std::string_view from_file);
};
} // namespace cs
