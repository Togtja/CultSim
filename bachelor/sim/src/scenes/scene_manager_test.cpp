#include "mock_scene.h"
#include "scene_manager.h"

#include <doctest/doctest.h>

TEST_CASE("mock_scene_works")
{
    cs::test::MockScene ms(true, true);
    CHECK(ms.update_count == 0);
    CHECK(ms.draw_count == 0);
    CHECK(ms.exit_count == 0);
    CHECK(ms.enter_count == 0);
}

TEST_CASE("scene_manager_commands")
{
    /** Instance we are operating on */
    cs::SceneManager sm{};

    constexpr float fake_dt = 1.f / 60.f;

    SUBCASE("push and pop")
    {
        REQUIRE(sm.size() == 0);
        sm.push<cs::test::MockScene>(true, true);
        REQUIRE(sm.size() == 0);

        sm.update(fake_dt);
        REQUIRE(sm.size() == 1);

        sm.pop();
        REQUIRE(sm.size() == 1);
        sm.update(fake_dt);
        REQUIRE(sm.size() == 0);
    }

    SUBCASE("clear")
    {
        sm.push<cs::test::MockScene>(true, true);
        sm.push<cs::test::MockScene>(true, true);
        sm.push<cs::test::MockScene>(true, true);
        sm.push<cs::test::MockScene>(true, true);
        sm.update(fake_dt);
        REQUIRE(sm.size() == 4);

        sm.clear();
        REQUIRE(sm.size() == 4);
        sm.update(fake_dt);
        REQUIRE(sm.size() == 0);
    }
}

TEST_CASE("scene_manager_scene_ops")
{
    cs::SceneManager sm{};
    constexpr float fake_dt = 1.f / 60.f;

    SUBCASE("calls proper methods")
    {
        sm.push<cs::test::MockScene>(true, true);
        sm.update(fake_dt);
        auto* scene = dynamic_cast<cs::test::MockScene*>(sm.get_active_scene());

        REQUIRE(scene->enter_count == 1);
        REQUIRE(scene->update_count == 1);
        sm.draw();
        REQUIRE(scene->draw_count == 1);
    }
}
