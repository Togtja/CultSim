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
    m_vao = VaoBuilder().attribute(0, 2, GL_FLOAT, 0u).bind_attribute(0, 0).build();

    std::vector<glm::vec2> rect = {{-.5f, -.5f}, {-.5f, .5f}, {.5f, .5f}, {.5f, -.5f}};
}
} // namespace gfx
} // namespace cs
