#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/tags.h"
#include "entity/systems/need.h"

#include "doctest/doctest.h"

TEST_CASE("testing the needs system initialization")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::TAG_Food};

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
        CHECK(needs.needs.begin()->tags == cs::TAG_Food);
        CHECK(needs.needs.begin()->status == 100);

        // Check that there are no pressing_needs to begin with
        REQUIRE(needs.pressing_needs.size() == 0);
    });
}

TEST_CASE("testing the needs system update function")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::TAG_Food};

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
        CHECK(needs.needs.begin()->tags == cs::TAG_Food);

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
        CHECK(needs.needs.begin()->tags == cs::TAG_Food);

        // Check that the new stat matches our expectations
        CHECK(needs.needs.begin()->status == 100.f - (55.f * needs.needs.begin()->decay_rate));

        // Check that there is a new pressing_need
        REQUIRE(needs.pressing_needs.size() == 1);
    });
}

TEST_CASE("Testing outliers for dt")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::TAG_Food};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));

    auto need_system = new cs::system::Need(test_registry);
    auto view        = test_registry.view<cs::component::Needs>();

    need_system->update(101.f);
    view.each([](cs::component::Needs& needs) {
        // Check that need has not been removed
        REQUIRE(needs.needs.size() == 1);

        // Check that the new stat did not drop below 0
        CHECK(needs.needs.begin()->status == 0.f);

        // Check that there is a new pressing_need
        REQUIRE(needs.pressing_needs.size() == 1);

        needs.needs.begin()->status += 100.f;
    });

    // set status above maximum allowed value
    view.each([](cs::component::Needs& needs) {
        needs.needs.begin()->status += 10.f;
        REQUIRE(needs.needs.begin()->status == 110.f);
    });

    // Ensure that next update drops it back to reasonable accepted levels
    need_system->update(1.f);
    view.each([](cs::component::Needs& needs) {
        REQUIRE(needs.needs.begin()->status == 100.f);
        REQUIRE(needs.pressing_needs.size() == 0.f);
    });
}

TEST_CASE("pressing needs are correctly updated")
{
    entt::registry test_registry;

    auto agent         = test_registry.create();
    cs::ai::Need need1 = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::TAG_Food};
    cs::ai::Need need2 = {static_cast<std::string>("thirst"), 4.f, 100.f, 1.f, cs::TAG_Drink};
    cs::ai::Need need3 = {static_cast<std::string>("sad"), 1.f, 100.f, 1.f, cs::TAG_Joy};
    cs::ai::Need need4 = {static_cast<std::string>("tired"), 2.f, 100.f, 1.f, cs::TAG_Sleep};
    test_registry.assign<cs::component::Needs>(agent,
                                               std::vector<cs::ai::Need>({need1, need2, need3, need4}),
                                               std::vector<cs::ai::Need>({}));

    auto need_system = new cs::system::Need(test_registry);
    auto view        = test_registry.view<cs::component::Needs>();

    // Ensure that many needs are added into the needs list correctly and that many tests can be added to pressing needs
    need_system->update(50.f);
    view.each([](cs::component::Needs& needs) {
        REQUIRE(needs.needs.size() == 4);
        for (auto need : needs.needs)
        {
            REQUIRE(need.status == 50.f);
        }
        REQUIRE(needs.pressing_needs.size() == 4);
    });

    need_system->update(-50.f);
    view.each([](cs::component::Needs& needs) {
        for (auto need : needs.needs)
        {
            REQUIRE(need.status == 100.f);
        }
        REQUIRE(needs.pressing_needs.size() == 0);
    });
}
