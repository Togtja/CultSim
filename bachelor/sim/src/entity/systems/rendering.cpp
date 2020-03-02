#include "rendering.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include "gfx/ImGUI/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace cs::system
{
void Rendering::update(float dt)
{
    CS_AUTOTIMER(Rendering System);

    static glm::vec3 noseecolor{1.f, 0.f, 0.f};
    static glm::vec3 seecolor{0.f, 1.f, 0.f};

    auto& registry = *m_context.registry;
    registry.view<component::Sprite, component::Vision>().each([](component::Sprite& spr, const component::Vision& vis) {
        spr.color = glm::mix(noseecolor, seecolor, glm::clamp(glm::vec3(vis.seen.size() / 25.f), glm::vec3(0.f), glm::vec3(1.f)));
    });

    registry.view<component::Movement, component::Sprite>().each([](const component::Movement& mov, component::Sprite& sprite) {
        sprite.texture.bigrees = glm::atan(mov.direction.y, mov.direction.x) / 3.14f * 127.5f;
    });

    registry.view<entt::tag<"selected"_hs>, component::Sprite>().each([](entt::tag<"selected"_hs> _, component::Sprite& sprite) {
        sprite.color = {1.f, 1.f, 1.f};
    });

    registry.view<entt::tag<"hovered"_hs>, component::Sprite>().each([](entt::tag<"hovered"_hs> _, component::Sprite& sprite) {
        sprite.color = {1.f, 0.8f, 0.f};
    });

    auto pos_sprite_view = registry.view<component::Sprite, component::Position>();
    pos_sprite_view.each([](const component::Sprite& sprite, const component::Position& pos) {
        gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
    });
}
} // namespace cs::system
