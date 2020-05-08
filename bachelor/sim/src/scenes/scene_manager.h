#pragma once

#include "scene.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace cs
{
struct ApplicationContext;

/**
 * Manages scene changes and updates of the various scenes that exist in the application
 */
class SceneManager
{
private:
    /**
     * Represents the desired action for a command
     */
    enum class ECommandType : uint16_t
    {
        Nothing,
        Push,
        Pop,
        Clear
    };

    /**
     * A reified command to be executed later in order to not break the scene stack in the middle of a frame
     */
    struct Command
    {
        /** Used when Command is Push, otherwise nullptr */
        std::unique_ptr<IScene> new_scene{nullptr};

        ECommandType command_type{ECommandType::Nothing};

        Command(std::unique_ptr<IScene> scene, ECommandType type) : new_scene(std::move(scene)), command_type(type){};
    };

    /** Current stack of scenes managed by the scene manager */
    std::vector<std::unique_ptr<IScene>> m_scenestack{};

    /** Commands to alter the scene stack that have not been run yet */
    std::vector<Command> m_pending_commands{};

    /** The context we exist in */
    ApplicationContext* m_context{nullptr};

public:
    /**
     * Pushes a new Scene onto the stack
     *
     * @tparam Scene The Scene getting pushed onto the stack
     * @tparam CtorArgs The types of Constructor arguments for said scene
     *
     * @param args The constructor arguments for the scene
     */
    template<typename Scene, typename... CtorArgs>
    void push(CtorArgs&&... args)
    {
        m_pending_commands.emplace_back(std::make_unique<Scene>(std::forward<CtorArgs>(args)...), ECommandType::Push);
    }

    /**
     * Check if there are no active scenes
     *
     * @return True if empty, otherwise false
     */
    [[nodiscard]] bool empty() const;

    /**
     * Clear the stack of scenes
     */
    void clear();

    /**
     * Pop the last added scene from the scene stack
     */
    void pop();

    /**
     * Update all scenes and process pending scene commands
     *
     * @param dt Delta time since this function was last called
     */
    void update(float dt);

    /**
     * Draw all scenes until the end or a scene returns false
     */
    void draw();

    /**
     * Get the number of scenes currently on the stack
     *
     * @return Number of scenes on the stack
     */
    [[nodiscard]] unsigned size() const;

    /**
     * Get the currently active Scene
     *
     * @return returns the active scene, or nullptr if there is no active scene
     * @warning Do not store the Scene after a pop, it will be invalid
     */
    [[nodiscard]] IScene* get_active_scene() const;

    /**
     * Set the application context for this scene manager
     *
     * @note It will propagate to all created scenes
     * @param context The new, updated application context
     */
    void set_application_context(ApplicationContext& context);
};

} // namespace cs
