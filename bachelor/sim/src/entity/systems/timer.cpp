#include "timer.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Timer::update(float dt)
{
    CS_AUTOTIMER(Action System);

    auto view = m_registry.view<component::Timer>();
    view.each([this, dt](entt::entity e, component::Timer& timer) {
        timer.time_spent += dt;
        if (timer.time_spent > timer.time_to_complete)
        {
            timer.number_of_loops--;
            timer.on_complete(e,m_registry);
            if (timer.number_of_loops > 0)
            {
                timer.number_of_loops--;
            }
        }

        if (timer.number_of_loops == 0) 
        {
            m_registry.remove<component::Timer>(e);
        }
    });
}
} // namespace cs::system