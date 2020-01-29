#pragma once

namespace cs
{
namespace gfx
{
/**
 * Compile a shader from the provided shader source code
 *
 * @param source The source code of the shader
 * @param type The type of the shader
 * @return OpenGL Name of the newly created shader
 */
GLuint compile_shader(std::string_view source, GLenum type);

/**
 * Compiles a shader from the given file
 *
 * @param rpath The relative path to the file which contains the shader source code
 * @param type The type of the shader
 * @return OpenGL Name of the newly created shader
 */
GLuint fcompile_shader(std::string_view rpath, GLenum type);


/**
 * Convert a shader type to a human readable string
 *
 * @param type The type of the shader, for example GL_VERTEX_SHADER
 * @return The shader name as a human readable string
 */
std::string get_gl_shader_type_name(GLenum type);
} // namespace gfx
} // namespace cs
