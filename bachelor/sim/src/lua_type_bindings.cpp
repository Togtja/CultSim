#include "lua_type_bindings.h"
#include "entity/components/components.h"
#include "entity/components/need.h"
#include "entity/components/strategy.h"
#include "entity/components/tags.h"
#include "entity/systems/system.h"

namespace cs::lua
{
void bind_glm(sol::state_view lua)
{
    /** Vectors */
    lua.new_usertype<glm::vec2>("Vec2", "x", &glm::vec2::x, "y", &glm::vec2::y);
    lua.new_usertype<glm::vec3>("Vec3", "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z);
}

void bind_components(sol::state_view lua)
{
    /** Component Helpers */
    lua.new_usertype<ai::Need>("Need",
                               "name",
                               &ai::Need::name,
                               "weight",
                               &ai::Need::weight,
                               "status",
                               &ai::Need::status,
                               "decayRate",
                               &ai::Need::decay_rate,
                               "tags",
                               &ai::Need::tags);

    /** Components */
    lua.new_usertype<component::Position>("PositionComponent", "position", &component::Position::position);
    lua.new_usertype<component::Sprite>("SpriteComponent", "color", &component::Sprite::color); // ignoring texture for now
}

void bind_systems(sol::state_view lua)
{
    lua.new_usertype<system::ISystem>("ISystem", "update", &system::ISystem::update);
}

} // namespace cs::lua
