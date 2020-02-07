#include "rendering.h"
#include "components.h"

namespace cs::system
{
void Rendering::update(float dt)
{
    auto pos_sprite_view = m_registry.view<component::Position, component::Sprite>();
    pos_sprite_view.each([this](const component::Position& pos, const component::Sprite& sprite) {
        m_renderer.sprite().draw(pos.position, sprite.color, sprite.texture);
    });

}
} // namespace cs::system
