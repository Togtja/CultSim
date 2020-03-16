#include "need.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "spdlog/spdlog.h"

namespace cs::system
{
void Need::update(float dt)
{
    CS_AUTOTIMER(Need System);

    auto view = m_context.registry->view<component::Need>();
    view.each([dt](component::Need& needs) {
        for (auto& need : needs.needs)
        {
            // Ensure statuses cannot go below 0% or above 100%
            need.status = std::clamp(need.status - (need.decay_rate * dt), 0.f, 100.f);
            auto found = std::find(needs.vital_needs.begin(), needs.vital_needs.end(), need);
            // If the need does not exist in pressing needs
            if (found != needs.vital_needs.end())
            {
                found->status = need.status;
                // Remove it if it should not be there
                if (found->status > 50.f)
                {
                    needs.vital_needs.erase(found);
                }
            }
            // If the need does exist in pressing needs
            else
            {
                // Add it if it should be there
                if (need.status <= 50.f)
                {
                    needs.vital_needs.push_back(need);
                }
            }
        }
    });
}
} // namespace cs::system
