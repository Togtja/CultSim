#pragma once

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

    VaoBuilder& enable(unsigned index);

    VaoBuilder& vbo(unsigned vbo, unsigned binding, unsigned offset = 0u, unsigned stride = 0u);

    VaoBuilder& ebo(unsigned ebo);

    VaoBuilder& divisor(unsigned binding, unsigned divisor);

    unsigned build();
};
} // namespace gfx

} // namespace cs
