#include "glutil.h"
#include "filesystem.h"

#include <spdlog/spdlog.h>

namespace cs
{
namespace gfx
{
GLuint compile_shader(std::string_view source, GLenum type)
{
    /* Must extract c-string to comply with OpenGL interface */
    const char* source_cstr = source.data();
    GLuint shader           = glCreateShader(type);

    glShaderSource(shader, 1, &source_cstr, nullptr);
    glCompileShader(shader);

    /* Check for erros */
    int err, len;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &err);

    if (!err)
    {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        auto log = std::string(len, '\0');

        glGetShaderInfoLog(shader, len, &len, log.data());
        spdlog::error("{} shader compile error: {}", get_gl_shader_type_name(type).c_str(), log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint fcompile_shader(std::string_view rpath, GLenum type)
{
    /* Read the file contents */
    auto source = fs::read_file(rpath);

    /* Use other function to compile the extracted source, then clean up and return */
    GLuint shader = compile_shader(source.c_str(), type);

    return shader;
}

GLuint create_program(const std::vector<GLuint>& shaders)
{
    /* Create program */
    GLuint program = glCreateProgram();

    /* Attach all shaders that should be part of the program */
    for (auto shader : shaders)
    {
        glAttachShader(program, shader);
    }

    /* Link the program */
    glLinkProgram(program);

    /* Check for errors */
    int err, len;
    glGetProgramiv(program, GL_LINK_STATUS, &err);

    if (!err)
    {
        /* Get log length */
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        auto log = std::string(len, '\0');

        /* Get the log text and print it */
        glGetProgramInfoLog(program, len, &len, log.data());
        spdlog::error("program Link Error: {}", log);

        /* Cleanup and return an invalid GL Name */
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

std::string get_gl_shader_type_name(GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER: return "vertex"; break;
        case GL_GEOMETRY_SHADER: return "geometry"; break;
        case GL_TESS_CONTROL_SHADER: return "tesselation control"; break;
        case GL_TESS_EVALUATION_SHADER: return "tesselation evaluation"; break;
        case GL_FRAGMENT_SHADER: return "fragment"; break;
        case GL_COMPUTE_SHADER: return "compute"; break;
        default: return "invalid"; break;
    }
}

} // namespace gfx
} // namespace cs
