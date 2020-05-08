#pragma once

#include "common_helpers.h"
#include "gfx/window.h"
#include "l10n/lang_manager.h"

#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

#include <entt/signal/sigh.hpp>
#include <glm/vec2.hpp>
#include <robin_hood.h>
#include <sol/state_view.hpp>

namespace cs
{
/** Type used to hold the value of a preference */
using PreferenceVariant = std::variant<bool, int, float, std::string, glm::ivec2>;

/**
 * Defines a preference of some kind that can hold a value of supported preference types
 */
struct Preference
{
    std::string name{};
    std::string description{};
    PreferenceVariant value{};
};

/** Helper for creating visitors efficiently */
template<class... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};
template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

/**
 * General store and management for changing preferences in the application
 *
 * The preference manager should be the main hub of interaction when it comes to changing preferences as it handles value changes
 * properly. You can also subscribe to preference changes if you care about that.
 */
class PreferenceManager
{
private:
    /** The strings for different languages */
    Locale m_locale;

    /** Application window */
    Window& m_window;

    /** Current Lua state */
    sol::state_view m_lua;

    /** Window resolution */
    Preference m_resolution{"Resolution", "The resolution of your game window", glm::ivec2{1280, 720}};

    /** Fullscreen or not */
    Preference m_fullscreen{"Fullscreen", "Should the window be fullscreen or windowed", false};

    /** Localization language */
    Preference m_language{"Language", "Application language", "en"};

    /** General purpose storage for saved variables */
    robin_hood::unordered_map<std::string, std::string> m_saved_variables{};

    /** Signal for publishing preference changes */
    entt::sigh<void(const Preference&, const Preference&)> m_preference_changed{};

public:
    /**
     * Sink for subscribing to preference changes
     *
     * The first parameter is the old value of the preference, and the second one is the new preference value
     */
    entt::sink<void(const Preference&, const Preference&)> on_preference_changed{m_preference_changed};

public:
    explicit PreferenceManager(Window& window, sol::state_view lua_state);
    explicit PreferenceManager(Window& window, sol::state_view lua_state, std::string_view from_file);

    /**
     * Creates and show a preference UI
     */
    void show_ui();

    /**
     * Initialize and load preferences
     */
    void init();

    /**
     * Deinitialize and save preferences
     */
    void deinit();

    /**
     * Get the resolution of the application
     *
     * @return The resolution of the application
     */
    [[nodiscard]] const Preference& get_resolution() const;

    /**
     * Set the resolution of the application
     *
     * @param resolution
     */
    void set_resolution(const glm::ivec2& resolution);

    /**
     * Get the fullscreen mode of the application
     *
     * @return The fullscreen mode of the application
     */
    [[nodiscard]] const Preference& get_fullscreen() const;

    /**
     * Set the fullscreen mode of the application
     *
     * @param fullscreen
     */
    void set_fullscreen(bool fullscreen);

    /**
     * Get the language of the application
     *
     * @return The language of the application
     */
    [[nodiscard]] const Preference& get_language() const;

    /**
     * Set the language of the application
     *
     * @param language
     */
    void set_language(std::string_view language);

    /**
     * Get a string from the current locale
     *
     * @param id The key of the string to get from the locale
     * @return The string in the current locale
     */
    std::string_view get_string(std::string_view id);

private:
    /**
     * Load preferences from lua file
     */
    void load_from_lua();

    /**
     * Writes the preference to a valid line for used in save_to_lua
     *
     * @param preference The Preference to serialize
     * @return The preference set as a lua variable
     */
    std::string write_preference(const Preference& preference);

    /**
     * Save preferences to lua file
     */
    void save_to_lua();
};
} // namespace cs
