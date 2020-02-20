#include "rendering.h"
#include "entity/components/components.h"

#include "gfx/ImGUI/imgui.h"
#include <glm/gtc/type_ptr.hpp>

namespace cs::system
{
void Rendering::update(float dt)
{
    static glm::vec3 noseecolor{1.f, 0.f, 0.f};
    static glm::vec3 seecolor{0.f, 1.f, 0.f};

    m_registry.group<component::Sprite>(entt::get<component::Vision>)
        .each([](component::Sprite& spr, const component::Vision& vis) {
            spr.color = glm::mix(noseecolor, seecolor, glm::vec3(vis.seen.size() / 20.f));
        });

    auto pos_sprite_view = m_registry.view<component::Sprite, component::Position>();
    pos_sprite_view.each([this](const component::Sprite& sprite, const component::Position& pos) {
        m_renderer.sprite().draw(pos.position, sprite.color, sprite.texture);
    });
}
} // namespace cs::system
