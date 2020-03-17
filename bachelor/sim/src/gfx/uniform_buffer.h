#pragma once

#include <utility>
#include <type_traits>

namespace cs::gfx
{
template<typename T>
class UniformBuffer
{
public:
    using ValueType = T;

private:
    T m_value{};

    T* m_mapped_pointer = nullptr;

public:
    UniformBuffer() = default;
    UniformBuffer(const T& initial_value) : m_value(initial_value)
    {
    }
};
} // namespace cs::gfx
