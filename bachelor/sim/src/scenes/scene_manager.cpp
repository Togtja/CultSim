#include "scene_manager.h"

namespace cs
{
bool SceneManager::empty() const
{
    return m_scenestack.empty();
}

void SceneManager::clear()
{
    m_pending_commands.emplace_back(nullptr, ECommandType::Clear);
}

void SceneManager::pop()
{
    m_pending_commands.emplace_back(nullptr, ECommandType::Pop);
}

void SceneManager::update(float dt)
{
    for (auto& command : m_pending_commands)
    {
        switch (command.command_type)
        {
            case ECommandType::Push:
                command.new_scene->set_application_context(*m_context);
                command.new_scene->on_enter();
                m_scenestack.emplace_back(std::move(command.new_scene));
                break;
            case ECommandType::Pop:
                m_scenestack.back()->on_exit();
                m_scenestack.pop_back();
                break;
            case ECommandType::Clear:
                for (auto it = m_scenestack.rbegin(); it != m_scenestack.rend(); ++it)
                {
                    (*it)->on_exit();
                }
                m_scenestack.clear();
                break;
            default: break;
        }
    }

    m_pending_commands.clear();

    /** Update all scenes until the end or we hit a blocking scene */
    for (auto it = m_scenestack.rbegin(); it != m_scenestack.rend(); ++it)
    {
        if (!(*it)->update(dt))
        {
            break;
        }
    }
}

void SceneManager::draw()
{
    for (auto it = m_scenestack.rbegin(); it != m_scenestack.rend(); ++it)
    {
        if (!(*it)->draw())
        {
            break;
        }
    }
}

unsigned SceneManager::size() const
{
    return m_scenestack.size();
}

IScene* SceneManager::get_active_scene() const
{
    if (m_scenestack.empty())
    {
        return nullptr;
    }
    else
    {
        return m_scenestack.back().get();
    }
}

void SceneManager::set_application_context(ApplicationContext& context)
{
    m_context = &context;
    for (auto&& scene : m_scenestack)
    {
        scene->set_application_context(context);
    }
}
} // namespace cs
