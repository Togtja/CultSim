#include "ai/path_finding.h"
#include "entity/components/components.h"
#include "entity/effect.h"
#include "entity/factory.h"
#include "scenario_scene.h"

#include <spdlog/spdlog.h>

namespace cs
{
static void set_children_in_pregnancy(entt::registry& reg,
                                      RandomEngine& rng,
                                      component::Pregnancy& preg,
                                      component::Reproduction& incubator,
                                      component::Reproduction& non_incubator,
                                      entt::entity incubator_e,
                                      entt::entity non_incubator_e)
{
    preg = reg.assign<component::Pregnancy>(incubator_e);
    if (incubator.children_deviation > 0)
    {
        preg.children_in_pregnancy = std::round(rng.normal(incubator.mean_children_pp, incubator.children_deviation));
    }
    else
    {
        preg.children_in_pregnancy = non_incubator.mean_children_pp;
    }

    if (incubator.gestation_deviation > 0)
    {
        preg.gestation_period = std::round(rng.normal(incubator.average_gestation_period, incubator.gestation_deviation));
    }
    else
    {
        preg.gestation_period = incubator.average_gestation_period;
    }
    preg.parents.incubator     = incubator_e;
    preg.parents.non_incubator = non_incubator_e;
}

void ScenarioScene::bind_scenario_lua_functions()
{
    /** Helpful to make following code shorter and more readable, copying is fine here, it's just a pointer */
    auto lua = m_context->lua_state;

    /** Bind component type identifiers to the component table */
    sol::table component      = lua.create_table("component");
    component["position"]     = entt::type_info<component::Position>::id();
    component["movement"]     = entt::type_info<component::Movement>::id();
    component["sprite"]       = entt::type_info<component::Sprite>::id();
    component["vision"]       = entt::type_info<component::Vision>::id();
    component["tag"]          = entt::type_info<component::Tags>::id();
    component["need"]         = entt::type_info<component::Need>::id();
    component["reproduction"] = entt::type_info<component::Reproduction>::id();
    component["strategy"]     = entt::type_info<component::Strategy>::id();
    component["health"]       = entt::type_info<component::Health>::id();
    component["memory"]       = entt::type_info<component::Memory>::id();
    component["attack"]       = entt::type_info<component::Attack>::id();
    component["trait"]        = entt::type_info<component::Traits>::id();
    component["inventory"]    = entt::type_info<component::Inventory>::id();
    component["name"]         = entt::type_info<component::Name>::id();
    component["action"]       = entt::type_info<component::Action>::id();
    component["goal"]         = entt::type_info<component::Goal>::id();

#define REGISTER_LUA_COMPONENT(N) component["lua" #N] = entt::type_info<component::LuaComponent<N>>::id()
    REGISTER_LUA_COMPONENT(1);
    REGISTER_LUA_COMPONENT(2);
    REGISTER_LUA_COMPONENT(3);
    REGISTER_LUA_COMPONENT(4);
    REGISTER_LUA_COMPONENT(5);
    REGISTER_LUA_COMPONENT(6);
    REGISTER_LUA_COMPONENT(7);
    REGISTER_LUA_COMPONENT(8);
    REGISTER_LUA_COMPONENT(9);
    REGISTER_LUA_COMPONENT(10);
    REGISTER_LUA_COMPONENT(11);
    REGISTER_LUA_COMPONENT(12);
    REGISTER_LUA_COMPONENT(13);
    REGISTER_LUA_COMPONENT(14);
    REGISTER_LUA_COMPONENT(15);
    REGISTER_LUA_COMPONENT(16);
#undef REGISTER_LUA_COMPONENT

    /** Get component from Lua */
    sol::table cultsim = lua.create_table("cultsim");
    cultsim.set_function("get_component", [this](sol::this_state s, entt::entity e, uint32_t id) -> sol::object {
        if (!m_registry.valid(e))
        {
            return sol::nil;
        }

        switch (id)
        {
            case entt::type_info<component::Position>::id():
                if (m_registry.try_get<component::Position>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Position>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Movement>::id():
                if (m_registry.try_get<component::Movement>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Movement>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Sprite>::id():
                if (m_registry.try_get<component::Sprite>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Sprite>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Vision>::id():
                if (m_registry.try_get<component::Vision>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Vision>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Tags>::id():
                if (m_registry.try_get<component::Tags>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Tags>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Need>::id():
                if (m_registry.try_get<component::Need>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Need>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Reproduction>::id():
                if (m_registry.try_get<component::Reproduction>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Reproduction>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Strategy>::id():
                if (m_registry.try_get<component::Strategy>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Strategy>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Health>::id():
                if (m_registry.try_get<component::Health>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Health>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Memory>::id():
                if (m_registry.try_get<component::Memory>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Memory>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Attack>::id():
                if (m_registry.try_get<component::Attack>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Attack>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Inventory>::id():
                if (m_registry.try_get<component::Inventory>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Inventory>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Traits>::id():
                if (m_registry.try_get<component::Traits>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Traits>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
            case entt::type_info<component::Name>::id():
                if (m_registry.try_get<component::Name>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Name>(e));
                }
                spdlog::critical("target [{}] does not have that component [{}]", e, id);
                return sol::nil;
                /** Add cases for lua components */
#define LUA_COMPONENT_CASE(N)                                                                                                    \
    case entt::type_info<component::LuaComponent<N>>::id():                                                                      \
        if (m_registry.has<component::LuaComponent<N>>(e))                                                                       \
        {                                                                                                                        \
            return m_registry.get<component::LuaComponent<N>>(e).lua_data;                                                       \
        }                                                                                                                        \
        spdlog::critical("target [{}] does not have that component [{}]", e, id);                                                \
        return sol::nil;

                LUA_COMPONENT_CASE(1)
                LUA_COMPONENT_CASE(2)
                LUA_COMPONENT_CASE(3)
                LUA_COMPONENT_CASE(4)
                LUA_COMPONENT_CASE(5)
                LUA_COMPONENT_CASE(6)
                LUA_COMPONENT_CASE(7)
                LUA_COMPONENT_CASE(8)
                LUA_COMPONENT_CASE(9)
                LUA_COMPONENT_CASE(10)
                LUA_COMPONENT_CASE(11)
                LUA_COMPONENT_CASE(12)
                LUA_COMPONENT_CASE(13)
                LUA_COMPONENT_CASE(14)
                LUA_COMPONENT_CASE(15)
                LUA_COMPONENT_CASE(16)
                /** Clean up macro */
#undef LUA_COMPONENT_CASE

            case entt::type_info<component::Action>::id():
                if (m_registry.try_get<component::Action>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Action>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            case entt::type_info<component::Goal>::id():
                if (m_registry.try_get<component::Goal>(e))
                {
                    return sol::make_object(s, &m_registry.get<component::Goal>(e));
                }
                else
                {
                    spdlog::critical("target [{}] does not have that component [{}]", e, id);
                    return sol::nil;
                }
                break;
            default:
                spdlog::critical("can not find the component [{}] you are looking for", id);
                return sol::nil;
                break;
        }
    });

    cultsim.set_function("get_strategy",
                         [](sol::this_state s, const sol::object& strat_lua, std::string_view strat_name) -> sol::object {
                             const auto& strat = strat_lua.as<component::Strategy>();
                             for (const auto& i : strat.strategies)
                             {
                                 if (i.name == strat_name)
                                 {
                                     return sol::make_object(s, &i);
                                 }
                             }
                             return sol::nil;
                         });

    cultsim.set_function("get_need",
                         [](sol::this_state s, const sol::object& need_lua, std::string_view need_name) -> sol::object {
                             const auto& need = need_lua.as<component::Need>();
                             for (const auto& i : need.needs)
                             {
                                 if (i.name == need_name)
                                 {
                                     return sol::make_object(s, &i);
                                 }
                             }
                             return sol::nil;
                         });

    cultsim.set_function("get_goal",
                         [](sol::this_state s, const sol::object& goal_lua, std::string_view goal_name) -> sol::object {
                             const auto& goal = goal_lua.as<component::Goal>();
                             for (const auto& i : goal.goals)
                             {
                                 if (i.m_name == goal_name)
                                 {
                                     return sol::make_object(s, &i);
                                 }
                             }
                             return sol::nil;
                         });

    cultsim.set_function("get_acquired_traits", [this](sol::this_state s, entt::entity e) -> sol::object {
        if (auto trait = m_registry.try_get<component::Traits>(e); trait)
        {
            return sol::make_object(s, &trait->acquired_traits);
        }
        return sol::nil;
    });

    cultsim.set_function("add_acquired_trait", [this](entt::entity e, sol::table sol_trait) -> void {
        if (auto trait_c = m_registry.try_get<component::Traits>(e); trait_c)
        {
            const auto& trait = detail::get_trait(sol_trait);
            const auto it     = std::find(trait_c->acquired_traits.begin(), trait_c->acquired_traits.end(), trait);
            if (it == trait_c->acquired_traits.end())
            {
                trait_c->acquired_traits.push_back(trait);
                return;
            }
            spdlog::get("lua")->info("add_acquired_trait: the given trait is already acquired");
            return;
        }
        spdlog::get("lua")->warn("add_acquired_trait: the entity argument does not have the trait component");
    });

    cultsim.set_function("get_attainable_traits", [this](sol::this_state s, entt::entity e) -> sol::object {
        if (auto trait = m_registry.try_get<component::Traits>(e); trait)
        {
            return sol::make_object(s, &trait->attainable_traits);
        }
        return sol::nil;
    });

    cultsim.set_function("add_attainable_trait", [this](entt::entity e, sol::table sol_trait) -> void {
        if (auto trait_c = m_registry.try_get<component::Traits>(e); trait_c)
        {
            const auto& trait = detail::get_trait(sol_trait);
            const auto it     = std::find(trait_c->attainable_traits.begin(), trait_c->attainable_traits.end(), trait);
            if (it == trait_c->attainable_traits.end())
            {
                trait_c->attainable_traits.push_back(trait);
                return;
            }
            spdlog::get("lua")->info("add_attainable_trait: the given trait is already attainable");
            return;
        }
        spdlog::get("lua")->warn("add_attainable_trait: the entity argument does not have the trait component");
    });

    cultsim.set_function("remove_component", [this](entt::entity e, uint32_t id) {
        switch (id)
        {
            case entt::type_info<component::Position>::id(): m_registry.remove_if_exists<component::Position>(e); break;
            case entt::type_info<component::Movement>::id(): m_registry.remove_if_exists<component::Movement>(e); break;
            case entt::type_info<component::Sprite>::id(): m_registry.remove_if_exists<component::Sprite>(e); break;
            case entt::type_info<component::Vision>::id(): m_registry.remove_if_exists<component::Vision>(e); break;
            case entt::type_info<component::Tags>::id(): m_registry.remove_if_exists<component::Tags>(e); break;
            case entt::type_info<component::Need>::id(): m_registry.remove_if_exists<component::Need>(e); break;
            case entt::type_info<component::Reproduction>::id(): m_registry.remove_if_exists<component::Reproduction>(e); break;
            case entt::type_info<component::Strategy>::id(): m_registry.remove_if_exists<component::Strategy>(e); break;
            case entt::type_info<component::Health>::id(): m_registry.remove_if_exists<component::Health>(e); break;
            case entt::type_info<component::Memory>::id(): m_registry.remove_if_exists<component::Memory>(e); break;
            case entt::type_info<component::Action>::id(): m_registry.remove_if_exists<component::Action>(e); break;
            case entt::type_info<component::Goal>::id(): m_registry.remove_if_exists<component::Goal>(e); break;
            case entt::type_info<component::Traits>::id():
                if (auto traits = m_registry.try_get<component::Traits>(e); traits)
                {
                    effect::unaffect_traits(e, *traits);
                    m_registry.remove<component::Traits>(e);
                };
                break;
            case entt::type_info<component::Inventory>::id(): m_registry.remove_if_exists<component::Inventory>(e); break;
            default: break;
        }
    });

    cultsim.set_function("add_component", [this](sol::this_state s, entt::entity e, uint32_t id) -> sol::object {
        switch (id)
        {
            case entt::type_info<component::Position>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Position>(e));
            case entt::type_info<component::Movement>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Movement>(e));
            case entt::type_info<component::Sprite>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Sprite>(e));
            case entt::type_info<component::Vision>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Vision>(e));
            case entt::type_info<component::Tags>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Tags>(e));
            case entt::type_info<component::Need>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Need>(e));
            case entt::type_info<component::Reproduction>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Reproduction>(e));
            case entt::type_info<component::Strategy>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Strategy>(e));
            case entt::type_info<component::Health>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Health>(e));
            case entt::type_info<component::Memory>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Memory>(e));
            case entt::type_info<component::Inventory>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Inventory>(e));
            case entt::type_info<component::Action>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Action>(e));
            case entt::type_info<component::Goal>::id():
                return sol::make_object(s, m_registry.assign_or_replace<component::Goal>(e));
            default: return sol::nil;
        }
    });

    /** Helper action to modify an entity need */
    cultsim.set_function("modify_need", [this](entt::entity e, ETag need_tags, float delta) {
        if (auto* needs = m_registry.try_get<component::Need>(e); needs)
        {
            for (auto& need : needs->needs)
            {
                if (need.tags & need_tags)
                {
                    need.status += delta;
                }
            }
        }
    });

    cultsim.set_function("add_to_inventory", [this](entt::entity owner, entt::entity target) {
        if (auto inventory = m_registry.try_get<component::Inventory>(owner); inventory)
        {
            if (inventory->size < inventory->max_size)
            {
                auto tags = m_registry.try_get<component::Tags>(target);
                m_dispatcher.enqueue<event::EntityPickedUp>(event::EntityPickedUp{owner, target, tags->tags});
                tags->tags = ETag(tags->tags | TAG_Inventory);
                inventory->contents.push_back(target);
            }
        }
    });

    cultsim.set_function("remove_from_inventory", [this](entt::entity owner, entt::entity target) {
        if (auto inventory = m_registry.try_get<component::Inventory>(owner); inventory)
        {
            int i = 0;
            for (auto& content : inventory->contents)
            {
                if (content == target)
                {
                    inventory->contents.erase(inventory->contents.begin() + i);
                }
                i++;
            }
        }
    });

    /** Apply Damage */
    cultsim.set_function("apply_basic_damage", [this](entt::entity e, float damage) {
        if (auto* health = m_registry.try_get<component::Health>(e); health)
        {
            health->health -= damage;
        }
    });

    /* Function to allow lua to connect to events */
    cultsim.set_function("connect", [this](const std::string& event_name, sol::function func) {
        auto handle        = std::make_unique<LuaEventHandle>();
        handle->func       = func;
        handle->connection = m_lua_ebinder.at(event_name)(m_dispatcher, handle->func);
        m_lua_event_handlers.emplace_back(std::move(handle));
    });

    /** Send notification */
    cultsim.set_function("notify", [this](std::string title, std::string information, float duration = 4.f) {
        m_notifications.push_back({std::move(title), std::move(information), -duration});
    });

    /** Spawn entity functions */
    cultsim.set_function("spawn", [this](const std::string& entity_type) {
        const auto& final_path = m_scenario.script_path + "/entities/" + entity_type + ".lua";

        const auto e = spawn_entity(m_registry, m_context->lua_state, final_path);

        m_dispatcher.enqueue<event::EntitySpawned>(event::EntitySpawned{entity_type, e});
        return e;
    });

    cultsim.set_function("spawn_at", [this](const std::string& entity_type, glm::vec2 position) {
        const auto& final_path = m_scenario.script_path + "/entities/" + entity_type + ".lua";

        const auto e = spawn_entity(m_registry, m_context->lua_state, final_path, position);

        m_dispatcher.enqueue<event::EntitySpawned>(event::EntitySpawned{entity_type, e, position});
        return e;
    });

    /** Destroy entity */
    cultsim.set_function("kill", [this](entt::entity e) {
        if (!m_registry.valid(e))
        {
            spdlog::get("agent")->critical("trying to kill an invalid agent");
            return;
        }

        m_registry.assign<component::Delete>(e);
        if (auto tags = m_registry.try_get<component::Tags>(e); tags)
        {
            tags->tags = static_cast<ETag>(tags->tags | ETag::TAG_Delete);
        }
    });

    /** Move to position */
    cultsim.set_function("move_to", [this](entt::entity e, glm::vec3 goal) {
        auto&& [movement, position] = m_registry.try_get<component::Movement, component::Position>(e);
        if (movement && position)
        {
            if (!ai::find_path_astar(position->position, goal, movement->desired_position, m_scenario.bounds))
            {
                spdlog::get("agent")->debug("could not create path to target");
            };
        }
    });

    cultsim.set_function("distance", [](glm::vec3 a, glm::vec3 b) { return glm::distance(a, b); });

    cultsim.set_function("entity_distance", [this](sol::this_state s, entt::entity a, entt::entity b) -> sol::object {
        const auto pos_a = m_registry.try_get<component::Position>(a);
        const auto pos_b = m_registry.try_get<component::Position>(b);
        if (pos_a && pos_b)
        {
            return sol::make_object(s, glm::distance(pos_a->position, pos_b->position));
        }
        return sol::nil;
    });

    cultsim.set_function("has_tags", [this](entt::entity e, ETag tags) {
        if (auto t_comp = m_registry.try_get<component::Tags>(e); t_comp)
        {
            return (t_comp->tags & tags) == tags;
        }

        spdlog::get("default")->error("The entity does not have a tag component.");
        return false;
    });

    cultsim.set_function("set_tags", [this](entt::entity e, ETag tags) {
        if (auto t_comp = m_registry.try_get<component::Tags>(e); t_comp)
        {
            t_comp->tags = ETag(t_comp->tags | tags);
        }
        spdlog::get("default")->error("The entity does not have a tag component.");
        return false;
    });

    cultsim.set_function("has_set_flags", [](gob::Action& action, uint32_t flags) { return (action.flags & flags) == flags; });

    cultsim.set_function("set_flags", [](gob::Action& action, uint32_t flags) { action.flags |= flags; });

    /** Check entity validity */
    cultsim.set_function("is_valid", [this](entt::entity e) { return m_registry.valid(e); });

    /** Impregnate */
    cultsim.set_function("impregnate", [this](entt::entity father, entt::entity mother) {
        /** Figure out "type" of mother and spawn a child based on that */

        /** Give a Pregnancy component to the mother */
        auto* rc_m = m_registry.try_get<component::Reproduction>(mother);
        auto* rc_f = m_registry.try_get<component::Reproduction>(father);
        if (rc_m && rc_f)
        {
            if (!m_rng.trigger(rc_m->fertility) || !m_rng.trigger(rc_f->fertility))
            {
                /** No pregnancy */
                return;
            }
            if (auto* again = m_registry.try_get<component::Pregnancy>(mother); again)
            {
                /** Can't get pregnant twice */
                return;
            }

            component::Pregnancy preg;
            if (rc_f->incubator == component::Reproduction::ESex::Female)
            {
                set_children_in_pregnancy(m_registry, m_rng, preg, *rc_m, *rc_f, mother, father);
            }
            else
            {
                set_children_in_pregnancy(m_registry, m_rng, preg, *rc_f, *rc_m, father, mother);
            }

            if (preg.children_in_pregnancy < 1)
            {
                preg.children_in_pregnancy = 1;
            }
        }
    });

    /** Generate a random name */
    cultsim.set_function("generate_name", [this](const std::string& ethnicity, bool is_male) {
        return m_name_generator.generate(ethnicity, is_male, m_rng);
    });

    /** Allow Lua to use Views */
    cultsim.set_function("view", [this](sol::object types) {
        auto vec = types.as<std::vector<uint32_t>>();
        return m_registry.runtime_view(vec.begin(), vec.end());
    });
}
} // namespace cs
