#include "requirement.h"

namespace cs::action
{
void IRequirement::set_entity(entt::entity entity)
{
    owner = entity;
}
} // namespace cs::action
