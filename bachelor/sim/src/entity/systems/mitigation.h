#include "system.h"

#include <vector>

namespace cs::system
{
class Mitigation : public ISystem
{
private:
public:
    using ISystem::ISystem;

    void update(float dt) override;
};
} // namespace cs::system
