#include "vao_builder.h"

#include <glad/glad.h>

namespace cs
{
namespace gfx
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
} // namespace gfx
} // namespace cs
