#include "common_helpers.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"
#include "mitigation.h"
#include "need.h"

#include "doctest/doctest.h"
#include "spdlog/spdlog.h"

TEST_CASE("Testing that system does not run if pressing needs are empty")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::tags::TAG_Food};
    cs::action::Action action{static_cast<std::string>("eat"),
                          cs::tags::TAG_Find,
                          5.f,
                          0.f,
                          []() { spdlog::warn("We failed to finish action: eat"); },
                          []() { spdlog::warn("We finished action: eat"); },
                          {}};

    cs::ai::Strategy strategy = {static_cast<std::string>("eat food"),
                             0,
                             {},
                             cs::tags::TAG_Food,
                             std::vector<cs::action::Action>{std::move(action)}};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));
    test_registry.assign<cs::component::Strategies>(agent, std::vector<cs::ai::Strategy>({strategy}), std::vector<cs::ai::Strategy>{});
    test_registry.assign<cs::component::Tags>(agent, cs::tags::TAG_Food);

    auto need_system = new cs::system::Need(test_registry);
    auto mitigation_system = new cs::system::Mitigation(test_registry);

    need_system->update(50.f);
    auto view = test_registry.view < cs::component::Needs, cs::component::Strategies, cs::component::Tags>();
    REQUIRE(view.size() == 1);
    view.each([](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) {
        // Check that need has been added
        REQUIRE(needs.needs.size() == 1);
        REQUIRE(needs.pressing_needs.size() == 1);

        // Check that the strategy has been added
        REQUIRE(strategies.strategies.size() == 1);
        CHECK(strategies.staged_strategies.size() == 0);
    });

    mitigation_system->update(1.f);
    view.each([](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) 
        {
        REQUIRE(!needs.pressing_needs.empty());
        REQUIRE(strategies.strategies.size() == 1);
        REQUIRE(strategies.staged_strategies.size() ==1);


        });

}
