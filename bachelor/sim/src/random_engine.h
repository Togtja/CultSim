#pragma once

#include <algorithm>
#include <random>
#include <type_traits>

#include <pcg/pcg_random.hpp>

namespace cs
{
/**
 * Utility class for providing random number generation
 */
class RandomEngine
{
private:
    /** Random generator */
    pcg32 m_random_generator;

public:
    RandomEngine() : m_random_generator(pcg_extras::seed_seq_from<std::random_device>())
    {
    }

    explicit RandomEngine(uint64_t seed) : m_random_generator(seed)
    {
    }

    RandomEngine(const RandomEngine&) = delete;
    RandomEngine& operator=(const RandomEngine&) = delete;

    /**
     * Create a uniform randomly distributed number in the given range
     *
     * @param min The minimum value, inclusive
     * @param max The maximum value, inclusive
     */
    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    std::decay_t<T> uniform(T min, T max)
    {
        if constexpr (std::is_integral_v<T>)
        {
            std::uniform_int_distribution<int> distribution(min, max);
            return distribution(m_random_generator);
        }
        else
        {
            std::uniform_real_distribution<double> distribution(min, max);
            return distribution(m_random_generator);
        }
    }

    /**
     * Create a normal distribution around [mean] with given standard [deviation]
     *
     * @param mean The mean value
     * @param deviation The standard deviation around the mean
     */
    double normal(double mean, double deviation = 5.0);

    /**
     * Return true or false with given probability
     *
     * @param probability The chance of returning true in range 0 - 1
     */
    bool trigger(double probability);

    /**
     * Simulate rolling a dice
     *
     * @param sides The number of sides on the dice
     * @param rolls The number of rolls to perform
     * @return The sum of all dice rolls
     */
    int roll(int sides, int rolls);

    /**
     * Pick a random element from the given container that support subscripting
     *
     * @param container The container to pick from
     */
    template<typename T>
    decltype(auto) pick(T& container)
    {
        return container[uniform<unsigned>(0, container.size() - 1)];
    }

    /**
     * Pick a random element from the given container that support subscripting
     *
     * @param container The container to pick from
     */
    template<typename T>
    decltype(auto) pick(const T& container) const
    {
        return container[uniform<unsigned>(0, container.size() - 1)];
    }
};
} // namespace cs
