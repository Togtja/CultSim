#pragma once
#include "components/components.h"

namespace cs::effect
{
/**
 * Given a trait component run the user typed affect function on all acquired traits
 *
 * @note only runs on the acquired traits
 * @param e The Entity you want it to run on
 * @param traits The trait component that belongs to e
 */
void affect_traits(entt::entity e, const component::Traits& traits);

/**
 * Given a trait component run the user typed unaffect function on all acquired traits
 *
 * @note only runs on the acquired traits
 * @param e The Entity you want it to run on
 * @param traits The trait component that belongs to e
 */
void unaffect_traits(entt::entity e, const component::Traits& traits);
} // namespace cs::effect
