#include "factory.h"
#include "components/components.h"
#include "filesystem/filesystem.h"
#include "gfx/renderer.h"

#include <entt/meta/factory.hpp>
#include <spdlog/spdlog.h>

namespace cs
{
namespace detail
{
/** Map of component factories */
static robin_hood::unordered_map<std::string, std::function<bool(entt::entity, entt::registry&, sol::table)>>
    s_component_factories{{"PositionComponent", spawn_position_component},
                          {"MovementComponent", spawn_movement_component},
                          {"SpriteComponent", spawn_sprite_component},
                          {"VisionComponent", spawn_vision_component},
                          {"TagComponent", spawn_tag_component},
                          {"NeedComponent", spawn_need_component},
                          {"ReproductionComponent", spawn_reproduction_component},
                          {"StrategyComponent", spawn_strategy_component},
                          {"HealthComponent", spawn_health_component}};

bool spawn_position_component(entt::entity e, entt::registry& reg, sol::table table)
{
    reg.assign_or_replace<component::Position>(e, glm::vec3(table["position"].get<glm::vec2>(), 0.f));
    return true;
}

bool spawn_movement_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& movement = reg.assign_or_replace<component::Movement>(e);
    movement.speed = table["speed"];
    return true;
}

bool spawn_sprite_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& spr = reg.assign_or_replace<component::Sprite>(e);

    /** Only assign texture if provided as a string */
    if (table["texture"].get_type() == sol::type::string)
    {
        spr.texture = gfx::get_renderer().sprite().get_texture(table["texture"].get<std::string>());
        spdlog::get("agent")->info("texture not provided for SpriteComponent");
    }

    spr.color = table["color"].get<glm::vec3>();
    return true;
}

bool spawn_vision_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& vis  = reg.assign_or_replace<component::Vision>(e);
    vis.radius = table["radius"].get<float>();
    vis.fov    = table["fov"].get<uint8_t>();

    return true;
}

bool spawn_tag_component(entt::entity e, entt::registry& reg, sol::table table)
{
    reg.assign_or_replace<component::Tags>(e, table["tags"].get<ETag>());
    return true;
}

bool spawn_need_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& need = reg.assign_or_replace<component::Needs>(e);

    const auto& required_needs = table["needs"].get_or<std::vector<sol::table>>({});
    for (const auto& need_table : required_needs)
    {
        need.needs.push_back(ai::Need{need_table["name"].get<std::string>(),
                                      need_table["weight"].get<float>(),
                                      need_table["status"].get<float>(),
                                      need_table["decay_rate"].get<float>(),
                                      need_table["vitality"].get<float>(),
                                      need_table["tags"].get<ETag>()});
    }

    return true;
}

bool spawn_reproduction_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& repl = reg.assign_or_replace<component::Reproduction>(e);

    repl.sex          = table["sex"].get<component::Reproduction::ESex>();
    repl.max_children = table["max_children"].get<int>();

    return true;
}

bool spawn_strategy_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& strat = reg.assign_or_replace<component::Strategies>(e);

    /** Populate all strategies */
    const auto& available_strategies = table["strategies"].get_or<std::vector<sol::table>>({});
    for (const auto& strategy_table : available_strategies)
    {
        ai::Strategy strategy{};
        strategy.name = strategy_table["name"].get<std::string>();
        strategy.tags = strategy_table["tags"].get<ETag>();

        /** Get the actions for this strategy */
        const auto& actions = strategy_table["actions"].get_or<std::vector<sol::table>>({});
        for (const auto& action_table : actions)
        {
            strategy.actions.push_back(action::Action{action_table["name"].get<std::string>(),
                                                      action_table["requirements"].get<ETag>(),
                                                      action_table["time_to_complete"].get<float>()});

            strategy.actions.back().success = action_table["success"];
        }

        strat.strategies.push_back(strategy);
    }

    return true;
}

bool spawn_health_component(entt::entity e, entt::registry& reg, sol::table table)
{
    reg.assign_or_replace<component::Health>(e,
                                             table["health"].get<float>(),
                                             table["tickdown_rate"].get<float>(),
                                             table["vital_needs"].get<ETag>());
    return true;
}

} // namespace detail

entt::entity spawn_entity(entt::registry& reg, sol::state_view lua, std::string_view entity, glm::vec2 position)
{
    auto out = reg.create();

    lua.script(fs::read_file(entity));
    sol::table entity_info = lua["entity"];

    /** Iterate components and spawn them with their factory functions */
    for (const auto& [k, v] : entity_info)
    {
        if (!detail::s_component_factories.at(k.as<std::string>().c_str())(out, reg, v))
        {
            spdlog::get("agent")->warn("failed to create {}. correct spelling?", k.as<std::string>());
        }
    }

    /** Set position if it has one */
    if (auto* pos = reg.try_get<component::Position>(out); pos)
    {
        pos->position = glm::vec3(position, 0.f);
    }

    spdlog::get("agent")->debug("created entity '{}' with {} components", entity, entity_info.size());

    return out;
}

} // namespace cs
