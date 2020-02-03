#include "debug_renderer.h"
#include "vao_builder.h"

#include <vector>

#include <glad/glad.h>

namespace cs
{
namespace gfx
{
DebugRenderer::DebugRenderer()
{
    m_vao = VaoBuilder().attribute(0, 0, 2, GL_FLOAT, 0u).build();
}
} // namespace gfx
} // namespace cs
