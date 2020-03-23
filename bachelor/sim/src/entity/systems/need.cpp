#include "need.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "entity/events.h"

#include <spdlog/spdlog.h>

namespace cs::system
{
void Need::update(float dt)
{
    CS_AUTOTIMER(Need System);

    auto view = m_context.registry->view<component::Need>();
    view.each([dt, this](entt::entity e, component::Need& needs) {
        for (auto& need : needs.needs)
        {
            // Ensure statuses cannot go below 0% or above 100%
            need.status = std::clamp(need.status - (need.decay_rate * dt), 0.f, 100.f);

            auto found = std::find(needs.vital_needs.begin(), needs.vital_needs.end(), need);
            // If the need does exist in pressing needs
            if (found != needs.vital_needs.end())
            {
                found->status = need.status;
                // Remove it if it should not be there
                if (found->status > 50.f)
                {
                    needs.vital_needs.erase(found);
                    m_context.dispatcher->enqueue<event::NeedNoLongerCritical>(
                        event::NeedNoLongerCritical{e, need.tags, need.status, need.weight});
                }
                // If there are no vital needs left and we are switching to Leisure Needs
                if (needs.vital_needs.empty())
                {
                    m_context.dispatcher->enqueue<event::SwitchNeedContext>(event::SwitchNeedContext{e});
                }
            }
            // If the need does not exist in pressing needs
            else
            {
                // Add it if it should be there
                if (need.status <= 50.f)
                {
                    // If vital needs are empty before adding this one
                    if (needs.vital_needs.empty())
                    {
                        m_context.dispatcher->enqueue<event::SwitchNeedContext>(event::SwitchNeedContext{e});
                    }
                    needs.vital_needs.push_back(need);
                    m_context.dispatcher->enqueue<event::NeedCritical>(
                        event::NeedCritical{e, need.tags, need.status, need.weight});
                }
            }
        }
    });
}

ISystem* Need::clone()
{
    return new Need(m_context);
}
} // namespace cs::system
