#pragma once
#include "system.h"

namespace cs::system
{
class Deletion : public ISystem
{
public:
    Deletion(SystemContext m_context) : ISystem(m_context){};
    ~Deletion(){};

    void update(float dt) override;
};
} // namespace cs::system