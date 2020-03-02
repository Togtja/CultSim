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
            timer.number_of_loops--;
            timer.time_spent = 0;
            timer.on_complete(e, *m_context.registry);
            if (timer.number_of_loops > 0)
            {
                timer.number_of_loops--;
            }
        }

        if (timer.number_of_loops == 0)
        {
            m_context.registry->remove<component::Timer>(e);
        }
    });
}
} // namespace cs::system
