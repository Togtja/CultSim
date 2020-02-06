#pragma once

#include "scene.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace cs
{
class SceneManager
{
private:
    enum class ECommandType : uint16_t
    {
        Nothing,
        Push,
        Pop,
        Clear
    };

    struct Command
    {
        /*Used when Command is Push*/
        std::unique_ptr<IScene> new_scene = nullptr;

        ECommandType command_type = ECommandType::Nothing;

        Command(std::unique_ptr<IScene> scene, ECommandType type) : new_scene(std::move(scene)), command_type(type){};
    };

    std::vector<std::unique_ptr<IScene>> m_scenestack = {};

    std::vector<Command> m_pending_commands = {};

public:

    /**
    * Pushes a new Scene onto the stack
    * @tparam IScene_ The Scene getting pushed onto the stack
    * @tparam CtorArgs_ The types of Constructor arguments for said scene
    *
    * @param args The contructor arguments for the scene
    */
    template<typename IScene_, typename... CtorArgs_>
    void push(CtorArgs_&&... args)
    {
        m_pending_commands.emplace_back(std::make_unique<IScene_>(std::forward<CtorArgs_>(args)...), ECommandType::Push);
    }

    /**
    * Checks if stack is empty
    */
    bool empty() const;

    /**
    * Clears the stack of scenes
    */
    void clear();

    void pop();

    void update(float dt);

    void draw();

    /**
    * gets our currently active Scene
    * @return returns the active scene, or nullptr if there is no active scene
    * @warning Do not store the Scene after a pop, it will be invalid
    */
    IScene* get_active_scene() const;
};

} // namespace cs
