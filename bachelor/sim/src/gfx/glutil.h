#pragma once

namespace cs
{
namespace gfx
{

/**
 * Convert a shader type to a human readable string
 *
 * @param type The type of the shader, for example GL_VERTEX_SHADER
 * @return The shader name as a human readable string
 */
std::string get_gl_shader_type_name(GLenum type);
} // namespace gfx
} // namespace cs
