#include "need.h"
#include "entity/components/components.h"

namespace cs::system
{
void Need::update(float dt)
{
    auto view = m_registry.view<component::Needs>();
    view.each([dt](component::Needs& needs) {
        for (auto need : needs.needs)
        {
            need.status -= need.decay_rate;
            if (need.status <= 50.f)
            {
                //If the need is not allready present in pressing_needs
                if (std::find(needs.pressing_needs.begin(), needs.pressing_needs.end(), need) != needs.pressing_needs.end())
                {
                    //pressing_needs are sorted later by the mitigation system
                    needs.pressing_needs.push_back(need);
                }
            }
        }
    });
}
} // namespace cs::system
