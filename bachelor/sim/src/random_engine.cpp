#include "random_engine.h"

namespace cs
{
double RandomEngine::normal(const double mean, const double deviation)
{
    std::normal_distribution<double> normalDistributor(mean, deviation);
    return normalDistributor(m_random_generator);
}

bool RandomEngine::trigger(const double probability)
{
    std::bernoulli_distribution probabilityTrigger(probability);
    return probabilityTrigger(m_random_generator);
}

int RandomEngine::roll(int sides, int rolls)
{
    int sum = 0;
    for (int i = 0; i < rolls; ++i)
    {
        sum += uniform(1, sides);
    }
    return sum;
}
} // namespace cs
