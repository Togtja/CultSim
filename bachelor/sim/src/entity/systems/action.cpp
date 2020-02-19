#include "action.h"
#include "entity/components/components.h"

#include <random>

#include "spdlog/spdlog.h"

namespace cs::system
{
void Action::update(float dt)
{
    auto view = m_registry.view<component::Strategies, component::Requirement>(entt::exclude<component::LocationRequirement()>);
    view.each([this, dt](entt::entity e, component::Strategies& strategies, component::Requirement& requirements) {
        if (requirements.staged_requirements.empty() && !strategies.staged_strategies.empty())
        {
            for (auto& strategy : strategies.staged_strategies)
            {
                if (!strategy.actions.empty())
                {
                    auto& action = strategy.actions.back();
                    if (!action.requirements != 0)
                    {
                        spdlog::warn("Pushing back requirement {}", action.requirements);
                        switch (action.requirements)
                        {
                            case tags::TAG_Location: m_registry.assign<component::LocationRequirement>(e, glm::vec3{20.f,20.f,0.f});
                        }
                        break;
                    }
                    else
                    {
                        action.time_spent += dt;
                        spdlog::info("Time spent: {}", action.time_spent);
                        if (action.time_spent >= action.time_to_complete)
                        {
                            std::random_device rand;
                            std::mt19937 gen(rand());
                            if (std::bernoulli_distribution(0.9)(gen))
                            {
                                action.success();
                            }
                            else
                            {
                                action.failure();
                            }
                            strategy.actions.pop_back();
                            break;
                        }
                    }
                }
                else
                {
                    continue;
                }
            }
        }
    });
}
} // namespace cs::system
