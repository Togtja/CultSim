#include "need.h"
#include "entity/components/components.h"
#include "spdlog/spdlog.h"

namespace cs::system
{
void Need::update(float dt)
{
    auto view = m_registry.view<component::Needs>();
    view.each([dt](component::Needs& needs) {
        for (auto& need : needs.needs)
        {
            // Ensure statuses cannot go below 0% or above 100%
            need.status = std::clamp(need.status - (need.decay_rate * dt), 0.f, 100.f);
            auto found  = std::find(needs.pressing_needs.begin(), needs.pressing_needs.end(), need);
            // If the need does not exist in pressing needs
            if (found != needs.pressing_needs.end())
            {
                // Remove it if it should not be there
                if (need.status >= 50.f)
                {
                    needs.pressing_needs.erase(found);
                }
            }
            // If the need does exist in pressing needs
            else
            {
                // Add it if it should be there
                if (need.status <= 50.f)
                {
                    needs.pressing_needs.push_back(need);
                }
            }
        }
    });
}
} // namespace cs::system