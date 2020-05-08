#include "vao_builder.h"

#include <glad/glad.h>

namespace cs::gfx
{
VaoBuilder::VaoBuilder()
{
    glCreateVertexArrays(1, &m_vao);
}

VaoBuilder::~VaoBuilder()
{
    if (m_vao != 0u)
    {
        glDeleteVertexArrays(1, &m_vao);
    }
}

VaoBuilder& VaoBuilder::attribute(unsigned index, unsigned binding, int size, unsigned type, unsigned offset, bool normalized)
{
    glVertexArrayAttribFormat(m_vao, index, size, type, normalized, offset);
    glVertexArrayAttribBinding(m_vao, index, binding);
    glEnableVertexArrayAttrib(m_vao, index);
    return *this;
}

VaoBuilder& VaoBuilder::iattribute(unsigned index, unsigned binding, int size, unsigned type, unsigned offset)
{
    glVertexArrayAttribIFormat(m_vao, index, size, type, offset);
    glVertexArrayAttribBinding(m_vao, index, binding);
    glEnableVertexArrayAttrib(m_vao, index);
    return *this;
}

VaoBuilder& VaoBuilder::vbo(unsigned vbo, unsigned binding, unsigned offset, unsigned stride)
{
    glVertexArrayVertexBuffer(m_vao, binding, vbo, offset, stride);
    return *this;
}

VaoBuilder& VaoBuilder::ebo(unsigned ebo)
{
    glVertexArrayElementBuffer(m_vao, ebo);
    return *this;
}

VaoBuilder& VaoBuilder::divisor(unsigned binding, unsigned divisor)
{
    glVertexArrayBindingDivisor(m_vao, binding, divisor);
    return *this;
}

VaoBuilder& VaoBuilder::bind()
{
    glBindVertexArray(m_vao);
    return *this;
}

unsigned VaoBuilder::build()
{
    auto tmp = m_vao;
    m_vao    = 0u;
    return tmp;
}
} // namespace cs::gfx
