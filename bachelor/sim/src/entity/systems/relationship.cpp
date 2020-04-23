#include "relationship.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Relationship::update(float dt)
{
    CS_AUTOTIMER(Relationship System);
}

ISystem* Relationship::clone()
{
    return new Relationship(m_context);
}
} // namespace cs::system
