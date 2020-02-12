#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/tags.h"
#include "entity/systems/need.h"

#include "doctest/doctest.h"

TEST_CASE("testing the needs system initialization")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::tags::TAG_Food};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));

    auto need_system = new cs::system::Need(test_registry);
    auto view        = test_registry.view<cs::component::Needs>();
    REQUIRE(view.size() == 1);
    view.each([](cs::component::Needs& needs) {
        // Check that need has been added
        REQUIRE(needs.needs.size() == 1);

        // Check that Need still has the same parameters
        CHECK(needs.needs.begin()->name == static_cast<std::string>("hunger"));
        CHECK(needs.needs.begin()->decay_rate == 1.f);
        CHECK(needs.needs.begin()->weight == 3.f);
        CHECK(needs.needs.begin()->tags == cs::tags::TAG_Food);
        CHECK(needs.needs.begin()->status == 100);

        // Check that there are no pressing_needs to begin with
        REQUIRE(needs.pressing_needs.size() == 0);
    });
}

TEST_CASE("testing the needs system update function")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::tags::TAG_Food};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));

    auto need_system = new cs::system::Need(test_registry);
    auto view        = test_registry.view<cs::component::Needs>();

    need_system->update(1.f);
    view.each([](cs::component::Needs& needs) {
        // Check that need has not been removed
        REQUIRE(needs.needs.size() == 1);

        // Check that Need still has the same parameters
        CHECK(needs.needs.begin()->name == static_cast<std::string>("hunger"));
        CHECK(needs.needs.begin()->decay_rate == 1.f);
        CHECK(needs.needs.begin()->weight == 3.f);
        CHECK(needs.needs.begin()->tags == cs::tags::TAG_Food);

        // Check that the new stat matches our expectations
        CHECK(needs.needs.begin()->status == 100.f - needs.needs.begin()->decay_rate);

        // Check that there are no pressing_needs
        REQUIRE(needs.pressing_needs.size() == 0);

        // Clean up after test
        needs.needs.begin()->status += needs.needs.begin()->decay_rate;
    });

    need_system->update(55.f);
    view.each([](cs::component::Needs& needs) {
        // Check that need has not been removed
        REQUIRE(needs.needs.size() == 1);

        // Check that Need still has the same parameters
        CHECK(needs.needs.begin()->name == static_cast<std::string>("hunger"));
        CHECK(needs.needs.begin()->decay_rate == 1.f);
        CHECK(needs.needs.begin()->weight == 3.f);
        CHECK(needs.needs.begin()->tags == cs::tags::TAG_Food);

        // Check that the new stat matches our expectations
        CHECK(needs.needs.begin()->status == 100.f - (55.f * needs.needs.begin()->decay_rate));

        // Check that there is a new pressing_need
        REQUIRE(needs.pressing_needs.size() == 1);
    });
}
