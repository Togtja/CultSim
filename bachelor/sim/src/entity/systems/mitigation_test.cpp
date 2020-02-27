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
    cs::RandomEngine rng{};

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::TAG_Food};
    cs::action::Action action{
        static_cast<std::string>("eat"),
        cs::TAG_Find,
        5.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) { spdlog::warn("We failed to finish action: eat"); },
        [](entt::entity e, entt::registry& r) { spdlog::warn("We finished action: eat"); },
        {}};

    cs::ai::Strategy strategy = {static_cast<std::string>("eat food"),
                                 0,
                                 {},
                                 cs::TAG_Food,
                                 std::vector<cs::action::Action>{std::move(action)}};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));
    test_registry.assign<cs::component::Strategies>(agent,
                                                    std::vector<cs::ai::Strategy>({strategy}),
                                                    std::vector<cs::ai::Strategy>{});
    test_registry.assign<cs::component::Tags>(agent, cs::TAG_Food);

    auto need_system       = new cs::system::Need({&test_registry, nullptr, &rng});
    auto mitigation_system = new cs::system::Mitigation({&test_registry, nullptr, &rng});

    need_system->update(50.f);
    auto view = test_registry.view<cs::component::Needs, cs::component::Strategies, cs::component::Tags>();
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
    view.each([](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) {
        REQUIRE(!needs.pressing_needs.empty());
        REQUIRE(strategies.strategies.size() == 1);
        REQUIRE(strategies.staged_strategies.size() == 1);
    });
}

TEST_CASE("Test case for mitigation system not adding strategies that do not match any tags")
{
    entt::registry test_registry;
    cs::RandomEngine rng{};

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 100.f, 1.f, cs::TAG_Food};
    cs::action::Action action{
        static_cast<std::string>("eat"),
        cs::TAG_Find,
        5.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) { spdlog::warn("We failed to finish action: eat"); },
        [](entt::entity e, entt::registry& r) { spdlog::warn("We finished action: eat"); },
        {}};

    cs::ai::Strategy strategy = {static_cast<std::string>("eat food"),
                                 0,
                                 {},
                                 cs::TAG_Food,
                                 std::vector<cs::action::Action>{std::move(action)}};

    cs::ai::Strategy strategy2 = {static_cast<std::string>("go sleep"),
                                  0,
                                  {},
                                  cs::TAG_Sleep,
                                  std::vector<cs::action::Action>{std::move(action)}};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));
    test_registry.assign<cs::component::Strategies>(agent,
                                                    std::vector<cs::ai::Strategy>({strategy, strategy2}),
                                                    std::vector<cs::ai::Strategy>{});
    test_registry.assign<cs::component::Tags>(agent, cs::TAG_Food);

    auto need_system       = new cs::system::Need({&test_registry, nullptr, &rng});
    auto mitigation_system = new cs::system::Mitigation({&test_registry, nullptr, &rng});

    need_system->update(50.f);
    mitigation_system->update(1.f);
    auto view = test_registry.view<cs::component::Needs, cs::component::Strategies, cs::component::Tags>();
    view.each([strategy](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) {
        REQUIRE(!needs.pressing_needs.empty());
        REQUIRE(strategies.strategies.size() == 2);
        REQUIRE(strategies.staged_strategies.size() == 1);
        REQUIRE(strategies.staged_strategies.back() == strategy);
    });
}

TEST_CASE("Test case to ensure strategies are ordered correctly")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger & thirst"),
                         3.f,
                         100.f,
                         1.f,
                         static_cast<cs::ETag>(cs::TAG_Food | cs::TAG_Drink)};
    cs::action::Action action{
        static_cast<std::string>("eat"),
        cs::TAG_Find,
        5.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) { spdlog::warn("We failed to finish action: eat"); },
        [](entt::entity e, entt::registry& r) { spdlog::warn("We finished action: eat"); },
        {}};

    cs::ai::Strategy strategy = {static_cast<std::string>("eat food"),
                                 0,
                                 {},
                                 cs::TAG_Food,
                                 std::vector<cs::action::Action>{std::move(action)}};

    cs::ai::Strategy strategy2 = {static_cast<std::string>("drink slushy"),
                                  0,
                                  {},
                                  static_cast<cs::ETag>(cs::TAG_Food | cs::TAG_Drink),
                                  std::vector<cs::action::Action>{std::move(action)}};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));
    test_registry.assign<cs::component::Strategies>(agent,
                                                    std::vector<cs::ai::Strategy>({strategy, strategy2}),
                                                    std::vector<cs::ai::Strategy>{});
    test_registry.assign<cs::component::Tags>(agent, cs::TAG_Food);

    auto need_system       = new cs::system::Need({&test_registry});
    auto mitigation_system = new cs::system::Mitigation({&test_registry});

    need_system->update(50.f);
    mitigation_system->update(1.f);
    auto view = test_registry.view<cs::component::Needs, cs::component::Strategies, cs::component::Tags>();
    view.each(
        [strategy, strategy2](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) {
            REQUIRE(!needs.pressing_needs.empty());
            REQUIRE(strategies.strategies.size() == 2);
            REQUIRE(strategies.staged_strategies.size() == 2);
            REQUIRE(strategies.staged_strategies.back() == strategy);
            REQUIRE(strategies.staged_strategies.front() == strategy2);
        });
}

TEST_CASE("Test case for strategies being removed if pressing needs becomes empty")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger & thirst"),
                         3.f,
                         100.f,
                         1.f,
                         static_cast<cs::ETag>(cs::TAG_Food | cs::TAG_Drink)};
    cs::action::Action action{
        static_cast<std::string>("eat"),
        cs::TAG_Find,
        5.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) { spdlog::warn("We failed to finish action: eat"); },
        [](entt::entity e, entt::registry& r) { spdlog::warn("We finished action: eat"); },
        {}};

    cs::ai::Strategy strategy = {static_cast<std::string>("eat food"),
                                 0,
                                 {},
                                 cs::TAG_Food,
                                 std::vector<cs::action::Action>{std::move(action)}};

    cs::ai::Strategy strategy2 = {static_cast<std::string>("drink slushy"),
                                  0,
                                  {},
                                  static_cast<cs::ETag>(cs::TAG_Food | cs::TAG_Drink),
                                  std::vector<cs::action::Action>{std::move(action)}};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need}), std::vector<cs::ai::Need>({}));
    test_registry.assign<cs::component::Strategies>(agent,
                                                    std::vector<cs::ai::Strategy>({strategy, strategy2}),
                                                    std::vector<cs::ai::Strategy>{});
    test_registry.assign<cs::component::Tags>(agent, cs::TAG_Food);

    auto need_system       = new cs::system::Need({&test_registry});
    auto mitigation_system = new cs::system::Mitigation({&test_registry});

    need_system->update(50.f);
    mitigation_system->update(1.f);
    auto view = test_registry.view<cs::component::Needs, cs::component::Strategies, cs::component::Tags>();
    view.each(
        [strategy, strategy2](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) {
            REQUIRE(!needs.pressing_needs.empty());
            REQUIRE(!strategies.staged_strategies.empty());
        });

    need_system->update(-50.f);
    mitigation_system->update(1.f);
    view.each(
        [strategy, strategy2](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) {
            REQUIRE(needs.pressing_needs.empty());
            REQUIRE(strategies.staged_strategies.empty());
        });
}

TEST_CASE("Test case that strategies change based on pressing_needs")
{
    entt::registry test_registry;

    auto agent        = test_registry.create();
    cs::ai::Need need = {static_cast<std::string>("hunger"), 3.f, 50.f, 0.f, static_cast<cs::ETag>(cs::TAG_Food)};

    cs::ai::Need need2 = {static_cast<std::string>("thirst"), 8.f, 100.f, 1.f, static_cast<cs::ETag>(cs::TAG_Drink)};

    cs::action::Action action{
        static_cast<std::string>("eat"),
        cs::TAG_Find,
        5.f,
        0.f,
        {},
        [](entt::entity e, entt::entity n, entt::registry& r) { spdlog::warn("We failed to finish action: eat"); },
        [](entt::entity e, entt::registry& r) { spdlog::warn("We finished action: eat"); },
        {}};

    cs::ai::Strategy strategy = {static_cast<std::string>("eat food"),
                                 0,
                                 {},
                                 cs::TAG_Food,
                                 std::vector<cs::action::Action>{std::move(action)}};

    cs::ai::Strategy strategy2 = {static_cast<std::string>("drink water"),
                                  0,
                                  {},
                                  static_cast<cs::ETag>(cs::TAG_Drink),
                                  std::vector<cs::action::Action>{std::move(action)}};

    test_registry.assign<cs::component::Needs>(agent, std::vector<cs::ai::Need>({need, need2}), std::vector<cs::ai::Need>({}));
    test_registry.assign<cs::component::Strategies>(agent,
                                                    std::vector<cs::ai::Strategy>({strategy, strategy2}),
                                                    std::vector<cs::ai::Strategy>{});
    test_registry.assign<cs::component::Tags>(agent, cs::TAG_Food);

    auto need_system       = new cs::system::Need({&test_registry});
    auto mitigation_system = new cs::system::Mitigation({&test_registry});

    need_system->update(1.f);
    mitigation_system->update(1.f);
    auto view = test_registry.view<cs::component::Needs, cs::component::Strategies, cs::component::Tags>();
    view.each(
        [strategy, strategy2](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) {
            REQUIRE(needs.pressing_needs.size() == 1);
            REQUIRE(strategies.staged_strategies.size() == 1);
            REQUIRE(strategies.staged_strategies.front() == strategy);
        });

    need_system->update(50.f);
    mitigation_system->update(1.f);
    view.each(
        [strategy, strategy2](cs::component::Needs& needs, cs::component::Strategies& strategies, cs::component::Tags& tags) {
            REQUIRE(needs.pressing_needs.size() == 2);
            REQUIRE(strategies.staged_strategies.size() == 1);
            REQUIRE(strategies.staged_strategies.front() == strategy2);
        });
}
