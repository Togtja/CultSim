#pragma once

#include <string>
#include <vector>

namespace cs::ai
{

struct Need
{
    std::string name{};

    float weight{};

    float status{};

<<<<<<< HEAD
    float decay_rate{};

=======
>>>>>>> 62eba4525e2695d08985798bfefdabb5a00346d7
    std::vector<std::string> tag{};
};

} // namespace cs::ai
