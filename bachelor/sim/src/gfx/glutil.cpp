#include "glutil.h"
#include "filesystem/filesystem.h"

#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include <gfx/stb_image.h>
#include <spdlog/spdlog.h>

namespace cs::gfx
{
GLuint compile_shader(std::string_view source, GLenum type)
{
    /* Must extract c-string to comply with OpenGL interface */
    const char* source_cstr = source.data();
    GLuint shader           = glCreateShader(type);

    glShaderSource(shader, 1, &source_cstr, nullptr);
    glCompileShader(shader);

    /* Check for erros */
    int err;
    int len;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &err);

    if (err == 0)
    {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        auto log = std::string(len, '\0');

        glGetShaderInfoLog(shader, len, &len, log.data());
        spdlog::get("graphics")->error("{} shader compile error: {}", get_gl_shader_type_name(type).c_str(), log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint fcompile_shader(std::string_view rpath, GLenum type)
{
    /* Read the file contents */
    const auto source = fs::read_file(rpath);

    /* Use other function to compile the extracted source, then clean up and return */
    GLuint shader = compile_shader(source.c_str(), type);

    return shader;
}

GLuint create_program(const std::vector<GLuint>& shaders)
{
    /* Create program */
    GLuint program = glCreateProgram();

    /* Attach all shaders that should be part of the program */
    for (const auto shader : shaders)
    {
        glAttachShader(program, shader);
    }

    /* Link the program */
    glLinkProgram(program);

    /* Check for errors */
    int err;
    int len;

    glGetProgramiv(program, GL_LINK_STATUS, &err);

    if (err == 0)
    {
        /* Get log length */
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        auto log = std::string(len, '\0');

        /* Get the log text and print it */
        glGetProgramInfoLog(program, len, &len, log.data());
        spdlog::get("graphics")->error("program Link Error: {}", log);

        /* Cleanup and return an invalid GL Name */
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

GLuint fcreate_program(const std::vector<ShaderFile>& shaders)
{
    /** Compile Shaders */
    std::vector<GLuint> compiled_shaders{};
    for (const auto& file : shaders)
    {
        compiled_shaders.push_back(fcompile_shader(file.path, file.type));
    }

    /** Link program and clean up shaders */
    GLuint program = create_program(compiled_shaders);
    for (const auto shader : compiled_shaders)
    {
        glDeleteShader(shader);
    }

    return program;
}

std::string get_gl_shader_type_name(GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER: return "vertex";
        case GL_GEOMETRY_SHADER: return "geometry";
        case GL_TESS_CONTROL_SHADER: return "tesselation control";
        case GL_TESS_EVALUATION_SHADER: return "tesselation evaluation";
        case GL_FRAGMENT_SHADER: return "fragment";
        case GL_COMPUTE_SHADER: return "compute";
        default: return "invalid";
    }
}

LoadedTexture load_texture(std::string_view rpath, bool flip_y)
{
    if (!fs::exists(rpath))
    {
        spdlog::get("graphics")->warn("texture does not exist: {}", rpath);
        return {};
    }

    /** Load bytes and parse as image */
    int c{};
    auto out         = LoadedTexture{};
    const auto bytes = fs::read_byte_file(rpath);
    stbi_set_flip_vertically_on_load(flip_y);
    const auto pixels = stbi_load_from_memory(bytes.data(), size_bytes(bytes), &out.width, &out.height, &c, STBI_rgb_alpha);

    /** Copy pixels into output */
    out.pixels.resize(out.width * out.height * STBI_rgb_alpha);
    memcpy(out.pixels.data(), pixels, out.pixels.size());

    stbi_image_free(pixels);
    return out;
}

std::vector<LoadedTexture> load_texture_partitioned(const char* fp, int xoffset, int yoffset, int w, int h, int cols, int count)
{
    return {};
}

#ifndef WIN32
#    define APIENTRY
#endif
static void APIENTRY gl_debug_callback(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const GLchar* message,
                                       const void* userParam)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_LOW: spdlog::get("graphics")->debug(message); break;
        case GL_DEBUG_SEVERITY_MEDIUM: spdlog::get("graphics")->warn(message); break;
        default: spdlog::get("graphics")->error(message); break;
    }
}

void create_debug_callback()
{
    glDebugMessageCallback(gl_debug_callback, nullptr);

    /* Only enable LOW -> HIGH priority debug messages. Ignore Notifications */
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, true);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, true);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, false);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
}

} // namespace cs::gfx
