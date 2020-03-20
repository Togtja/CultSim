#pragma once
#include "entity/events.h"
#include "system.h"
namespace cs::system
{
class Deletion : public ISystem
{
public:
    Deletion(SystemContext context) : ISystem(context)
    {
        m_context.dispatcher->sink<event::DeleteEntity>().connect<&Deletion::check_and_delete>(this);
    };

    ~Deletion()
    {
        m_context.dispatcher->sink<event::DeleteEntity>().disconnect<&Deletion::check_and_delete>(this);
    };

    void update(float dt) override;

    void check_and_delete(const event::DeleteEntity& event);
};
} // namespace cs::system
