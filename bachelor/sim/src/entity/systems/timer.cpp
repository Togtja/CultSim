#include "timer.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Timer::update(float dt)
{
    CS_AUTOTIMER(Action System);

    auto group = m_registry.group<component::Timer>();
    group.each([this, dt](entt::entity e, component::Timer& timer) {

    });
}
} // namespace cs::system