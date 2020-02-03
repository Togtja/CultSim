#include "primitives.h"

#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>

namespace cs
{
namespace gfx
{
PrimitiveCircle::PrimitiveCircle(float radius, int detail) : m_radius(radius), m_detail(detail)
{
}

std::tuple<std::vector<PrimitiveVertex>, std::vector<unsigned>> cs::gfx::PrimitiveCircle::generate_tris()
{
    std::vector<PrimitiveVertex> verts{};
    std::vector<unsigned> indices{};

    /** Add origin */
    verts.push_back({{0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}});

    for (int i = 0; i < m_detail; ++i)
    {
        /** Add current point using ratio */
        const auto ratio = i / static_cast<float>(m_detail);
        verts.push_back({{glm::cos(ratio * glm::two_pi<float>()), glm::sin(ratio * glm::two_pi<float>()), 0.f}, {1.f, 1.f, 1.f}});

        /** Add indices for center, current and next point along the circle */
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back((i + 1) % m_detail);
    }

    return {verts, indices};
}

std::tuple<std::vector<PrimitiveVertex>, std::vector<unsigned>> cs::gfx::PrimitiveCircle::generate_outline()
{
    std::vector<PrimitiveVertex> verts{};
    std::vector<unsigned> indices{};

    for (int i = 0; i < m_detail; ++i)
    {
        const auto ratio = i / static_cast<float>(m_detail);
        verts.push_back({{glm::cos(ratio * glm::two_pi<float>()), glm::sin(ratio * glm::two_pi<float>()), 0.f}, {1.f, 1.f, 1.f}});
        indices.push_back(i);
    }

    return {verts, indices};
}

} // namespace gfx

} // namespace cs
