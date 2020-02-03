#pragma once

#include "render_data.h"

#include <vector>
#include <tuple>

#include <glm/vec3.hpp>

namespace cs
{
namespace gfx
{
/**
 * Interface for primitive generation, override for shapes like grid, circle and other primitives
 */
class IPrimitive
{
public:
    /**
     * Generate the primitive vertices and indices for drawing as a filled triangle
     *
     * @return A vector of vertices and indices
     */
    virtual std::tuple<std::vector<PrimitiveVertex>, std::vector<unsigned>> generate_tris() = 0;

    /**
     * Generate the primitive vertices and indices for drawing as an outline line
     *
     * @return A vector of vertices and indices
     */
    virtual std::tuple<std::vector<PrimitiveVertex>, std::vector<unsigned>> generate_outline() = 0;
};

/**
 * Implementation of IPrimitive that generates a 2D circular shape
 */
class PrimitiveCircle : public IPrimitive
{
private:
    float m_radius = 10.f;
    int m_detail   = 32;

public:
    PrimitiveCircle() = default;
    PrimitiveCircle(float radius, int detail = 32);

    std::tuple<std::vector<PrimitiveVertex>, std::vector<unsigned>> generate_tris() override;
    std::tuple<std::vector<PrimitiveVertex>, std::vector<unsigned>> generate_outline() override;
};

class PrimitiveQuad : public IPrimitive
{
private:
    float m_side_length = 1.f;
    float m_ratio       = 1.f;

public:
    PrimitiveQuad(float side_length, float m_ratio = 1.f);
    ;

    std::tuple<std::vector<PrimitiveVertex>, std::vector<unsigned>> generate_tris() override;
    std::tuple<std::vector<PrimitiveVertex>, std::vector<unsigned>> generate_outline() override;
};

PrimitiveQuad::PrimitiveQuad(float side_length, float ratio);
} // namespace gfx
} // namespace cs
