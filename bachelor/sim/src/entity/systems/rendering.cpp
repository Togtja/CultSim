#include "rendering.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

#include <array>
#include <numeric>

#include "gfx/ImGUI/imgui.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

static constexpr std::array visualization_mode_names{"None", "Vision", "Needs", "Health"};

namespace cs::system
{
void Rendering::update(float dt)
{
    CS_AUTOTIMER(Rendering System);

    static glm::vec3 noseecolor{1.f, 0.f, 0.2f};
    static glm::vec3 seecolor{0.f, 0.9f, 0.2f};

    auto& registry = *m_context.registry;

    /** Select visualization mode */
    if (ImGui::TreeNode("Visualization Mode"))
    {
        ImGui::ListBoxHeader("##VisualizationListBox");
        for (int i = 0; i < visualization_mode_names.size(); ++i)
        {
            if (ImGui::Selectable(visualization_mode_names[i], static_cast<int>(m_visualization_mode) == i))
            {
                m_visualization_mode = static_cast<EVisualizationMode>(i);
            }
        }
        ImGui::ListBoxFooter();
        ImGui::TreePop();
    }

    /** Visualize if needed */
    if (m_visualization_mode == EVisualizationMode::Vision)
    {
        registry.view<component::Sprite, component::Vision>().each([](component::Sprite& spr, const component::Vision& vis) {
            spr.color =
                glm::mix(noseecolor, seecolor, glm::clamp(glm::vec3(vis.seen.size() / 25.f), glm::vec3(0.f), glm::vec3(1.f)));
        });
    }
    else if (m_visualization_mode == EVisualizationMode::Needs)
    {
        registry.view<component::Sprite, component::Need>().each([](component::Sprite& spr, const component::Need& needs) {
            auto sum = std::accumulate(needs.needs.begin(), needs.needs.end(), 0.f, [](const auto& a, const auto& b) {
                return a + b.status;
            });

            /** Compute average */
            sum /= needs.needs.size() / 100.f;
            spr.color = glm::mix(noseecolor, seecolor, glm::clamp(glm::vec3(sum), glm::vec3(0.f), glm::vec3(1.f)));
        });
    }
    else if (m_visualization_mode == EVisualizationMode::Health)
    {
        registry.view<component::Sprite, component::Health>().each([](component::Sprite& spr, const component::Health& hp) {
            spr.color = glm::mix(noseecolor, seecolor, glm::clamp(glm::vec3(hp.health / 100.f), glm::vec3(0.f), glm::vec3(1.f)));
        });
    }

    /** Apply rotation on sprites from movement */
    registry.view<component::Movement, component::Sprite>().each([](const component::Movement& mov, component::Sprite& sprite) {
        sprite.texture.bigrees = glm::atan(mov.direction.y, mov.direction.x) / 3.14f * 127.5f;
    });

    /** Submit sprites to renderer, for each layer */
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
}
} // namespace cs::system
