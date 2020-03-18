#include "action.h"
#include "entity/components/components.h"
#include "entity/systems/mitigation.h"
#include "entity/systems/need.h"
#include "random_engine.h"

#include <random>

#include "doctest/doctest.h"
#include "spdlog/spdlog.h"

TEST_CASE("Test case for regular action system setup")
{
    entt::registry test_registry;
    cs::RandomEngine rng{};

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, 1.f, cs::TAG_Food};
    cs::action::Action action{static_cast<std::string>("eat"), cs::TAG_Find, 5.f, 0.f, {}, {}, {}, {}};

    cs::ai::Strategy strategy =
        {static_cast<std::string>("eat food"), 0, {}, cs::TAG_Food, {}, {}, std::vector<cs::action::Action>{action}};

    test_registry.assign<cs::component::Need>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));
    test_registry.assign<cs::component::Strategy>(agent,
                                                  std::vector<cs::ai::Strategy>({strategy}),
                                                  std::vector<cs::ai::Strategy>{});
    test_registry.assign<cs::component::Tags>(agent, cs::TAG_Food);
    entt::dispatcher dispatcher{};

    auto need_system       = new cs::system::Need({&test_registry, &dispatcher, &rng});
    auto mitigation_system = new cs::system::Mitigation({&test_registry, &dispatcher, &rng});
    auto action_system     = new cs::system::Action({&test_registry, &dispatcher, &rng});

    need_system->update(51.f);
    mitigation_system->update(1.f);
    action_system->update(1.f);

    need_system->update(1.f);
    mitigation_system->update(1.f);
    action_system->update(1.f);

    auto view = test_registry.view<cs::component::Need, cs::component::Strategy, cs::component::Tags>();
    view.each([action](cs::component::Need& needs, cs::component::Strategy& strategies, cs::component::Tags& tags) {
        REQUIRE(strategies.staged_strategies.front().actions.size() == 1);
        REQUIRE(strategies.staged_strategies.front().actions.back() == action);
    });
}
