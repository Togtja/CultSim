#include "timer.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Timer::update(float dt)
{
    CS_AUTOTIMER(Timer System);

    auto view = m_context.registry->view<component::Timer>();
    view.each([this, dt](entt::entity e, component::Timer& timer) {
        timer.time_spent += dt;
        if (timer.time_spent > timer.time_to_complete)
        {
            /** on_complete - can either be lua or cpp function */
            if (timer.on_complete.index() == 0)
            {
                std::get<component::Timer::OnCompleteFunction>(timer.on_complete)(e, *m_context.registry);
            }
            else
            {
                std::get<sol::function>(timer.on_complete)(e);
            }

            /** Reset */
            timer.time_spent = 0.f;
            if (timer.number_of_loops > 0)
            {
                timer.number_of_loops--;
            }
        }

        /** Remove if we are out of loops */
        if (timer.number_of_loops == 0)
        {
            m_context.registry->remove<component::Timer>(e);
        }
    });
}

ISystem* Timer::clone()
{
    return new Timer(m_context);
}
} // namespace cs::system
