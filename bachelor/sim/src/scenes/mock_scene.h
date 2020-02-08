#pragma once

#include "scene.h"

namespace cs::test
{
/**
 * Scene used for testing of the scene manger to ensure it calls the proper functions
 */
class MockScene : public IScene
{
private:
    bool m_blocking_update = false;
    bool m_blocking_draw   = false;

public:
    unsigned enter_count  = 0u;
    unsigned exit_count   = 0u;
    unsigned update_count = 0u;
    unsigned draw_count   = 0u;

    MockScene(bool block_update, bool block_draw) : m_blocking_update(!block_update), m_blocking_draw(!block_draw)
    {
    }

    void on_enter() override
    {
        ++enter_count;
    }

    void on_exit() override
    {
        ++exit_count;
    }

    bool update(float dt) override
    {
        ++update_count;
        return m_blocking_update;
    }

    bool draw() override
    {
        ++draw_count;
        return m_blocking_draw;
    }
};
} // namespace cs::test
