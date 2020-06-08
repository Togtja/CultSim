#include "gobbas.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void GOBBAS::update(float dt)
{
    CS_AUTOTIMER(Goal Oriented Action System);

    auto view = m_context.registry->view<component::Action>();
    view.each([](entt::entity e, component::Action& action) {
        bool finished{};
        std::string error{};

        /** While we are not finished or get an error, keep working */
        if (action.current_action_sequence.m_run_actions.index() == 0)
        {
            finished = std::get<sol::function>(action.current_action_sequence.m_run_actions)(e, error).get<bool>();
        }
        else
        {
            finished = std::get<std::function<bool(gob::ActionSequence*, entt::entity, std::string*)>>(
                action.current_action_sequence.m_run_actions)(&action.current_action_sequence, e, &error);
        }

        /** If we get an error or finish, clear our current action sequence */
        if (finished)
        {
            action.current_action_sequence = gob::ActionSequence{};
        }
        if (error != "")
        {
            spdlog::get("agent")->error(error);
            action.current_action_sequence = gob::ActionSequence{};
        }
    });
}

ISystem* GOBBAS::clone()
{
    return new GOBBAS(m_context);
}
} // namespace cs::system
