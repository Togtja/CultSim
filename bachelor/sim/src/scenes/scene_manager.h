#pragma once

#include "scene.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace cs
{
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
        std::unique_ptr<IScene> new_scene = nullptr;

        ECommandType command_type = ECommandType::Nothing;

        Command(std::unique_ptr<IScene> scene, ECommandType type) : new_scene(std::move(scene)), command_type(type){};
    };

    std::vector<std::unique_ptr<IScene>> m_scenestack = {};

    std::vector<Command> m_pending_commands = {};

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
     * Push an existing Scene onto the stack
     *
     * @param scene The scene to push to the stack
     * @note Ownership must be transferred
     */
    void push(std::unique_ptr<IScene> scene);

    /**
    * Checks if stack is empty
    */
    [[nodiscard]] bool empty() const;

    /**
    * Clears the stack of scenes
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
    * Get the currently active Scene
     *
    * @return returns the active scene, or nullptr if there is no active scene
    * @warning Do not store the Scene after a pop, it will be invalid
    */
    [[nodiscard]] IScene* get_active_scene() const;
};

} // namespace cs
