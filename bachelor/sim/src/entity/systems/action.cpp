#include "action.h"
#include "entity/components/components.h"

#include <random>

#include "spdlog/spdlog.h"

namespace cs::system
{
void Action::update(float dt)
{
    auto view = m_registry.view<component::Strategies, component::Requirement>();
    view.each([this, dt](component::Strategies& strategies, component::Requirement& requirements) {
        if (requirements.staged_requirements.empty())
        {
            for (auto& strategy : strategies.staged_strategies)
            {
                if (!strategy.actions.empty())
                {
                    auto& action = strategy.actions.back();
                    if (!action.requirements.empty())
                    {
                        spdlog::warn("Pushing back requirement {}", action.requirements.back()->name);
                        requirements.staged_requirements.push_back(std::move(action.requirements.back()));
                        requirements.staged_requirements.back()->init();
                        action.requirements.pop_back();
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
                        }
                    }
                }
            }
        }
        else
        {
            spdlog::error("We are in the requirements");
            if (requirements.staged_requirements.back()->predicate)
            {
                requirements.staged_requirements.pop_back();
            }
        }
    });
}
} // namespace cs::system
