#include "debug_renderer.h"
#include "vao_builder.h"
#include "render_data.h"

#include <vector>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace cs
{
namespace gfx
{
DebugRenderer::DebugRenderer(Camera& camera) : m_camera(camera)
{
    m_vao = VaoBuilder().attribute(0, 0, 2, GL_FLOAT, 0u).build();

    glCreateBuffers(1, &m_linevbo);
    glNamedBufferStorage(m_linevbo, sizeof(PrimitiveVertex) * 2u, nullptr, GL_DYNAMIC_STORAGE_BIT);
}
}
} // namespace gfx
} // namespace cs
