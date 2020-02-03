#include "debug_renderer.h"

#include <vector>

#include <glad/glad.h>

namespace cs
{
namespace gfx
{
DebugRenderer::DebugRenderer()
{
    std::vector<glm::vec2> rect = {{-.5f, -.5f}, {-.5f, .5f}, {.5f, .5f}, {.5f, -.5f}};
}
} // namespace gfx
} // namespace cs
