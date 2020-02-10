#include "need.h"
#include "entity/components/components.h"

namespace cs::system
{
void Need::update(float dt)
{
    auto view = m_registry.view<component::Position, component::Movement>();
    view.each([dt](component::Needs& needs) {
        for (auto need : needs.needs)
        {
            need.status -= need.decay_rate;
            if (need.status <= 50.f)
            {
                //Send Event to Mitigation creation system
            }
        }
    });
}
} // namespace cs::system
