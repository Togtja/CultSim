#pragma once

#include <functional>

namespace cs
{
namespace gfx
{
class VaoBuilder
{
private:
    unsigned m_vao{};

public:
    VaoBuilder();
    VaoBuilder(const VaoBuilder&) = delete;
    VaoBuilder(VaoBuilder&&)      = delete;
    VaoBuilder& operator=(const VaoBuilder&) = delete;
    VaoBuilder& operator=(VaoBuilder&&) = delete;
    ~VaoBuilder();

    VaoBuilder& attribute(unsigned index, int size, unsigned type, unsigned offset, bool normalized = false);

    VaoBuilder& iattribute(unsigned index, int size, unsigned type, unsigned offset);

    VaoBuilder& bind_attribute(unsigned index, unsigned binding);

    VaoBuilder& vbo(unsigned vbo, unsigned binding, unsigned offset = 0u, unsigned stride = 0u);

    VaoBuilder& ebo(unsigned ebo);

    VaoBuilder& divisor(unsigned binding, unsigned divisor);

    VaoBuilder& bind();

    unsigned build();
};
} // namespace gfx

} // namespace cs
