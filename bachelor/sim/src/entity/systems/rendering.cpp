#include "rendering.h"
#include "constants.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"
#include "gfx/renderer.h"
#include "input/input_handler.h"

#include <array>
#include <numeric>

#include <gfx/ImGUI/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace cs::system
{
void Rendering::update(float dt)
{
    CS_AUTOTIMER(Rendering System);

    auto& registry = *m_context.registry;

    /** Apply rotation on sprites from movement */
    registry.view<component::Movement, component::Sprite>().each([](const component::Movement& mov, component::Sprite& sprite) {
        sprite.texture.bigrees = glm::atan(mov.direction.y, mov.direction.x) / 3.14f * 127.5f;
    });

    /** 2D Drawing */
    registry.view<component::Sprite, component::Position, entt::tag<"layer_0"_hs>>().less(
        [](const component::Sprite& sprite, const component::Position& pos) {
            gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
        });

    registry.view<component::Sprite, component::Position, entt::tag<"layer_1"_hs>>().less(
        [](const component::Sprite& sprite, const component::Position& pos) {
            gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
        });

    registry.view<component::Sprite, component::Position, entt::tag<"layer_2"_hs>>().less(
        [](const component::Sprite& sprite, const component::Position& pos) {
            gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
        });

    registry.view<component::Sprite, component::Position, entt::tag<"layer_3"_hs>>().less(
        [](const component::Sprite& sprite, const component::Position& pos) {
            gfx::get_renderer().sprite().draw(pos.position, sprite.color, sprite.texture);
        });

    /** 3D Drawing */
    registry.view<component::Sphere3D, component::Position>().less(
        [](const component::Sphere3D& sphere, const component::Position& pos) {
            gfx::get_renderer().raymarch().draw_sphere(pos.position, sphere.radius);
        });

    /** Selected / hovered */
    registry.view<component::Position, entt::tag<"selected"_hs>>().less([](const component::Position& pos) {
        gfx::get_renderer().debug().draw_circle(pos.position, 10.f, {0.f, 1.f, 0.f});
        gfx::get_renderer().debug().draw_rect(pos.position, {15.f, 15.f}, {0.f, 1.f, 0.f});
    });

    registry.view<component::Position, entt::tag<"hovered"_hs>>().less([](const component::Position& pos) {
        gfx::get_renderer().debug().draw_circle(pos.position, 10.f, {1.f, 1.f, 0.f});
    });
}

ISystem* Rendering::clone()
{
    return new Rendering(m_context);
}

void Rendering::update_imgui()
{
    static glm::vec3 light_dir{.8f, .4f, .2f};
    static glm::vec3 light_col{7.f, 5.f, 3.f};

    if (ImGui::TreeNode("Environment"))
    {
        if (ImGui::DragFloat3("Sun Direction", (float*)&light_dir, 0.01f, -1.f, 1.f))
        {
            gfx::get_renderer().set_sun_direction(light_dir);
        }

        if (ImGui::ColorEdit3("Sun Color", (float*)&light_col, ImGuiColorEditFlags_HDR))
        {
            gfx::get_renderer().set_sun_color({light_col, 1.f});
        }
        ImGui::TreePop();
    }
}
} // namespace cs::system
