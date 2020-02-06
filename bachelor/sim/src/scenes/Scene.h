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

    virtual void on_enter() = 0;

    virtual void on_exit() = 0;

    virtual bool update(float dt) = 0;

    /*
    * draw is responsible for drawing the scene
    * @return if it returns true, scenes below this one still draw
    */
    virtual bool draw() = 0;
};
} // namespace cs
