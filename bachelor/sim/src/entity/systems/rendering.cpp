#include "rendering.h"
#include "constants.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "gfx/renderer.h"

#include <array>
#include <numeric>

#include "gfx/ImGUI/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace cs::system
{
Rendering::Rendering(SystemContext context) : ISystem(context)
{
    /** Submit sprites to renderer, for each layer */
    m_shadow_texture              = gfx::get_renderer().sprite().get_texture("sprites/shadow_c.png");
    m_shadow_texture.material_idx = MATERIAL_IDX_NOSPEC;
}

void Rendering::update(float dt)
{
    CS_AUTOTIMER(Rendering System);

    auto& registry = *m_context.registry;

    /** Apply rotation on sprites from movement */
    //    registry.view<component::Movement, component::Sprite>().each([](const component::Movement& mov, component::Sprite&
    //    sprite) {
    //        sprite.texture.bigrees = glm::atan(mov.direction.y, mov.direction.x) / 3.14f * 127.5f;
    //    });

    //    registry.view<component::Sprite, component::Position, entt::tag<"layer_0"_hs>>().less(
    //        [](const component::Sprite& sprite, const component::Position& pos) {
    //            gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
    //        });

    //    registry.view<component::Sprite, component::Position, entt::tag<"layer_1"_hs>>().less(
    //        [](const component::Sprite& sprite, const component::Position& pos) {
    //            gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
    //        });

    //    registry.view<component::Sprite, component::Position, entt::tag<"layer_2"_hs>>().less(
    //        [](const component::Sprite& sprite, const component::Position& pos) {
    //            gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
    //        });

    //    registry.view<component::Sprite, component::Position, entt::tag<"layer_3"_hs>>().less(
    //        [](const component::Sprite& sprite, const component::Position& pos) {
    //            gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
    //        });

    //    /** Selected / hovered */
    //    registry.view<component::Position, entt::tag<"selected"_hs>>().less([](const component::Position& pos) {
    //        gfx::get_renderer().debug().draw_circle(pos.position, 10.f, {0.f, 1.f, 0.f});
    //        gfx::get_renderer().debug().draw_rect(pos.position, {15.f, 15.f}, {0.f, 1.f, 0.f});
    //    });

    //    registry.view<component::Position, entt::tag<"hovered"_hs>>().less([](const component::Position& pos) {
    //        gfx::get_renderer().debug().draw_circle(pos.position, 10.f, {1.f, 1.f, 0.f});
    //    });

    m_3d_renderer.display();
}

ISystem* Rendering::clone()
{
    return new Rendering(m_context);
}
} // namespace cs::system
