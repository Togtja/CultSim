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
            auto found = std::find(needs.pressing_needs.begin(), needs.pressing_needs.end(), need);
            //If the need does not exist in pressing needs
            if (found != needs.pressing_needs.end())
            {
                //Add it if it should be there
                if (need.status <= 50.f)
                {
                    needs.pressing_needs.emplace_back(need);
                }
            }
            //If the need does exist in pressing needs
            else
            {
                //Remove it if it should not be there
                if (need.status >= 50.f)
                {
                    needs.pressing_needs.erase(found);
                }
            }
        }
    });
}
} // namespace cs::system
