#include "mitigation.h"
#include "entity/components/need.h"
#include "entity/components/components.h"

namespace cs::system
{
void Mitigation::update(float dt)
{
    auto view = m_registry.view<component::Needs>();
    view.each([dt](component::Needs& needs)
        {
            //Put the most pressing needs to the front
            std::sort(needs.pressing_needs.begin(), needs.pressing_needs.end());


        });
}
} // namespace cs::system
