#pragma once

namespace cs
{
class IScene
{
public:
    IScene()                                   = default;
    IScene(const IScene&)                      = delete;
    IScene(IScene&& other) noexcept            = default;
    IScene& operator=(const IScene&)           = delete;
    IScene& operator=(IScene&& other) noexcept = default;
    virtual ~IScene() noexcept                 = default;

    /**
     * Called when the scene is entered
     */
    virtual void on_enter() = 0;

    /**
     * Called when the scene is exited
     */
    virtual void on_exit() = 0;

    /**
     * Called every frame and is meant for logic updates
     *
     * @param dt Delta time since previous call to update
     * @return true if this scene allow other scenes to update after it
     */
    virtual bool update(float dt) = 0;

    /**
     * Draw is responsible for drawing the scene
     *
     * @return if it returns true, scenes below this one still draw
     */
    virtual bool draw() = 0;
};
} // namespace cs
