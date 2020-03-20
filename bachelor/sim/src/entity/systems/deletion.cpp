#include "deletion.h"
#include "debug/auto_timer.h"
#include "entity/components/components.h"

namespace cs::system
{
void Deletion::initialize()
{
    m_context.dispatcher->sink<event::DeleteEntity>().connect<&Deletion::check_and_delete>(*this);
}

void Deletion::deinitialize()
{
    m_context.dispatcher->sink<event::DeleteEntity>().disconnect<&Deletion::check_and_delete>(*this);
}

void Deletion::update(float dt)
{
    CS_AUTOTIMER(Deletion System);

    auto& registry = *m_context.registry;

    auto view = registry.view<component::Delete>();
    view.less([this](entt::entity e) { m_context.registry->destroy(e); });
}

void Deletion::check_and_delete(const event::DeleteEntity& event)
{
    spdlog::get("agent")->critical("We are deleting enity {}", event.entity);
    m_context.registry->assign<component::Delete>(event.entity);
}
} // namespace cs::system
