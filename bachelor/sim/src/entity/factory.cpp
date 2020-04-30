#include "factory.h"
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
                          {"HealthComponent", spawn_health_component},
                          {"AttackComponent", spawn_attack_component},
                          {"MemoryComponent", spawn_memory_component},
                          {"TimerComponent", spawn_timer_component},
                          {"AgeComponent", spawn_age_component},
                          {"InventoryComponent", spawn_inventory_component},
                          {"TraitComponent", spawn_trait_component},
                          {"NameComponent", spawn_name_component},
                          {"RelationshipComponent", spawn_relationship_component}};

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
        if (table["normal"].get_type() == sol::type::string)
        {
            spr.texture =
                gfx::get_renderer().sprite().get_texture(table["texture"].get<std::string>(), table["normal"].get<std::string>());
        }
        else
        {
            spr.texture = gfx::get_renderer().sprite().get_texture(table["texture"].get<std::string>(), {});
        }
    }
    else
    {
        spdlog::get("agent")->info("texture not provided for SpriteComponent, or is not a string");
    }

    spr.color = table["color"].get<glm::vec3>();

    /** Get scale if specified */
    if (table["scale"].get_type() == sol::type::number)
    {
        spr.texture.scale = table["scale"].get<uint8_t>();
    }

    /** Get rotation if specified */
    if (table["rotation"].get_type() == sol::type::number)
    {
        spr.texture.bigrees = table["rotation"].get<uint8_t>();
    }

    /** Get layer if specified, or assign default layer 0 */
    if (table["layer"].get_type() == sol::type::number)
    {
        const auto layer_name = table["layer"].get<int64_t>();
        switch (layer_name)
        {
            case 1: reg.assign<entt::tag<"layer_1"_hs>>(e); break;
            case 2: reg.assign<entt::tag<"layer_2"_hs>>(e); break;
            case 3: reg.assign<entt::tag<"layer_3"_hs>>(e); break;
            default: reg.assign<entt::tag<"layer_0"_hs>>(e); break;
        }
    }
    else
    {
        reg.assign<entt::tag<"layer_0"_hs>>(e);
    }

    return true;
}

bool spawn_vision_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& vis  = reg.assign_or_replace<component::Vision>(e);
    vis.radius = table["radius"].get<float>();
    vis.fov    = table["fov"].get<uint8_t>();

    return true;
}

bool spawn_name_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& name       = reg.assign_or_replace<component::Name>(e);
    name.entity_type = table["entity_type"].get<std::string>();

    return true;
}

bool spawn_tag_component(entt::entity e, entt::registry& reg, sol::table table)
{
    reg.assign_or_replace<component::Tags>(e, table["tags"].get<ETag>());
    return true;
}

bool spawn_need_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& need = reg.assign_or_replace<component::Need>(e);

    const auto& required_needs = table["required_needs"].get_or<std::vector<sol::table>>({});
    const auto& leisure_needs  = table["leisure_needs"].get_or<std::vector<sol::table>>({});

    for (const auto& need_table : required_needs)
    {
        auto need_struct = ai::Need{need_table["name"].get<std::string>(),
                                    need_table["weight"].get<float>(),
                                    need_table["status"].get<float>(),
                                    need_table["decay_rate"].get<float>(),
                                    need_table["vitality"].get<float>(),
                                    need_table["tags"].get<ETag>()};

        if (need_table["weight_func"].get_type() == sol::type::function)
        {
            need_struct.weight_func = need_table["weight_func"].get<sol::function>();
        }
        need.needs.push_back(need_struct);
    }

    for (const auto& need_table : leisure_needs)
    {
        need.leisure_needs.push_back(ai::Need{need_table["name"].get<std::string>(),
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

    repl.sex = table["sex"].get<component::Reproduction::ESex>();

    repl.mean_children_pp = table["mean_offspring"].get<int>();
    if (table["offspring_deviation"].get_type() == sol::type::number)
    {
        repl.children_deviation = table["offspring_deviation"].get<float>();
    }

    if (table["fertility"].get_type() == sol::type::number)
    {
        repl.fertility     = table["fertility"].get<float>();
        repl.has_fertility = false;
    }
    else if (table["fertility"].get_type() == sol::type::boolean && table["fertility"].get<bool>())
    {
        if (!table["start_fertility"].valid() || !table["peak_fertility"].valid() || !table["end_fertility"].valid())
        {
            spdlog::get("lua")->critical("fertility is true, so you need to set start, peak and end fertility");
        }
        repl.has_fertility   = true;
        repl.start_fertility = table["start_fertility"].get<float>();
        repl.peak_fertility  = table["peak_fertility"].get<float>();
        repl.end_fertility   = table["end_fertility"].get<float>();
    }

    if (table["lays_eggs"].get_type() == sol::type::boolean)
    {
        repl.lays_eggs = table["lays_eggs"].get<bool>();
        if (repl.lays_eggs && table["egg_type"].valid())
        {
            repl.egg_type = table["egg_type"].get<std::string>();
        }
    }

    if (table["incubator"].valid())
    {
        repl.incubator = table["incubator"].get<component::Reproduction::ESex>();
    }
    if (table["average_gestation"].get_type() == sol::type::number)
    {
        repl.average_gestation_period = table["average_gestation"].get<float>();
        repl.gestation_deviation      = table["gestation_deviation"].get<float>();
    }
    return true;
} // namespace detail

bool spawn_strategy_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& strat = reg.assign_or_replace<component::Strategy>(e);

    /** Populate all strategies */
    const auto& available_strategies = table["strategies"].get_or<std::vector<sol::table>>({});
    for (const auto& strategy_table : available_strategies)
    {
        ai::Strategy strategy{};
        strategy.name        = strategy_table["name"].get<std::string>();
        strategy.tags        = strategy_table["tags"].get<ETag>();
        strategy.target_tags = strategy_table["target_tags"].get<ETag>();
        /** Get the actions for this strategy */
        const auto& actions = strategy_table["actions"].get_or<std::vector<sol::table>>({});
        for (const auto& action_table : actions)
        {
            strategy.actions.push_back(action::Action{action_table["name"].get<std::string>(),
                                                      action_table["requirements"].get<ETag>(),
                                                      action_table["time_to_complete"].get<float>(),
                                                      action_table["success_chance"].get<float>(),
                                                      action_table["success"].get<sol::function>(),
                                                      action_table["failure"].get<sol::function>()});

            if (action_table["abort"].get_type() == sol::type::function)
            {
                strategy.actions.back().abort = action_table["abort"].get<sol::function>();
            }

            if (action_table["targets_self"].get_type() == sol::type::boolean && action_table["targets_self"].get<bool>())
            {
                strategy.actions.back().target = e;
            }
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

bool spawn_attack_component(entt::entity e, entt::registry& reg, sol::table table)
{
    reg.assign_or_replace<component::Attack>(e, table["damage"].get<float>());
    return true;
}

bool spawn_memory_component(entt::entity e, entt::registry& reg, sol::table table)
{
    const std::vector<ETag>& allowed_memories = table["allowed_memories"].get_or<std::vector<ETag>>({});

    auto& memory_component =
        reg.assign_or_replace<component::Memory>(e, table["max_memories"].get<int>(), table["max_retention_time"].get<float>());

    /** Add allowed memories to memory compnent */
    for (auto tag : allowed_memories)
    {
        memory_component.memory_container.emplace_back(tag);
    }

    return true;
}

bool spawn_timer_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& timer_component           = reg.assign_or_replace<component::Timer>(e, table["time_to_complete"].get<float>());
    timer_component.number_of_loops = table["number_of_loops"].get<int>();
    timer_component.on_complete     = table["on_complete"].get<sol::function>();
    return true;
}

bool spawn_age_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& age_component                   = reg.assign_or_replace<component::Age>(e);
    age_component.average_life_expectancy = table["life_expectancy"].get<float>();
    return true;
}

bool spawn_inventory_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& inventory_component    = reg.assign_or_replace<component::Inventory>(e);
    inventory_component.max_size = table["max_size"].get<uint16_t>();
    return true;
}

component::detail::Trait get_trait(sol::table traits)
{
    component::detail::Trait trait;
    trait.name          = traits["name"];
    trait.desc          = traits["desc"];
    trait.affect        = traits["affect"];
    trait.remove_affect = traits["unaffect"];

    if (traits["can_inherit"].get_type() == sol::type::boolean)
    {
        trait.can_inherit = traits["can_inherit"];
        if (traits["inherit_chance"].get_type() == sol::type::number)
        {
            trait.inherit_chance = traits["inherit_chance"];
        }
    }

    if (traits["mutable"].get_type() == sol::type::boolean)
    {
        trait.can_mutate = traits["can_mutate"];
        if (traits["mutate_chance"].get_type() == sol::type::number)
        {
            trait.mutate_chance = traits["mutate_chance"];
        }
    }

    if (traits["attain_condition"].get_type() == sol::type::function)
    {
        trait.attain = traits["attain_condition"];
    }
    else
    {
        // Give a default function/bool
        spdlog::warn("No attain condition");
    }

    if (traits["lose_condition"].get_type() == sol::type::function)
    {
        trait.lose = traits["lose_condition"];
    }
    else
    {
        // Give a default function/bool
        spdlog::warn("No lose condition");
    }
    return trait;
}

bool spawn_trait_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& trait_comp = reg.assign_or_replace<component::Traits>(e);
    // TODO: Assign the traits that the component has as default
    const auto& available_default = table["start_traits"].get_or<std::vector<sol::table>>({});
    for (const auto& traits : available_default)
    {
        if (traits["trait"].get_type() == sol::type::table && traits["chance"].get_type() == sol::type::number)
        {
            if (reg.ctx<RandomEngine*>()->trigger(traits["chance"]))
            {
                trait_comp.start_traits.push_back(get_trait(traits["trait"]));
            }
        }
        else
        {
            trait_comp.start_traits.push_back(get_trait(traits));
        }
    }

    const auto& available_attainable = table["attainable_traits"].get_or<std::vector<sol::table>>({});
    for (const auto& traits : available_attainable)
    {
        trait_comp.attainable_traits.push_back(get_trait(traits));
    }
    return true;
}

bool spawn_relationship_component(entt::entity e, entt::registry& reg, sol::table table)
{
    auto& rel_comp = reg.assign_or_replace<component::Relationship>(e);

    return true;
}
} // namespace detail

entt::entity spawn_entity(entt::registry& reg, sol::state_view lua, std::string_view entity, glm::vec2 position)
{
    auto out = spawn_entity(reg, lua, entity);

    /** Set position if it has one */
    if (auto* pos = reg.try_get<component::Position>(out); pos)
    {
        pos->position = glm::vec3(position, 0.f);
    }

    return out;
}

entt::entity spawn_entity(entt::registry& reg, sol::state_view lua, std::string_view entity)
{
    auto out = reg.create();
    reg.assign<component::Meta>(out, std::string(entity));

    lua.script(fs::read_file(entity));
    sol::table entity_info = lua["entity"];

    /** Iterate components and spawn them with their factory functions */
    uint32_t components = 0u;
    for (const auto& [k, v] : entity_info)
    {
        if (!detail::s_component_factories.at(k.as<std::string>().c_str())(out, reg, v))
        {
            spdlog::get("agent")->warn("failed to create {}. correct spelling?", k.as<std::string>());
        }
        ++components;
    }

    spdlog::get("agent")->debug("created entity '{}' with {} components", entity, components);

    return out;
}

} // namespace cs
