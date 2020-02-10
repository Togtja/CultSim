#include "need.h"
<<<<<<< HEAD
#include "entity/components/components.h"

namespace cs::system
{
void Need::update(float dt)
{
    auto view = m_registry.view<component::Position, component::Movement>();
    view.each([dt](component::Needs& needs) {
        for (auto need : needs.needs ) {
          need.status -= need.decay_rate;
        }
} // namespace cs::system
=======
>>>>>>> 62eba4525e2695d08985798bfefdabb5a00346d7
