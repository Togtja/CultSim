#pragma once

#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include <glad/glad.h>

namespace cs::gfx
{
/**
 * Data about a loaded texture with relevant information for further processing
 */
struct LoadedTexture
{
    int width                   = 0u;
    int height                  = 0u;
    std::vector<uint8_t> pixels = {};
};

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
 * Create and link a program from the two supplied shaders
 *
 * @param shaders The compiled shaders that should be linked in the program
 * @return OpenGL Name of the newly created program
 */
GLuint create_program(const std::vector<GLuint>& shaders);

/**
 * Convert a shader type to a human readable string
 *
 * @param type The type of the shader, for example GL_VERTEX_SHADER
 * @return The shader name as a human readable string
 */
std::string get_gl_shader_type_name(GLenum type);

/**
 * Load the texture at the given relative filepath
 *
 * @param rpath The relative filepath of the texture
 * @return Texture data with information and a vector of pixels laid out like: [R0,G0,B0,A0, R1,G1,B1,A1 ...]
 */
LoadedTexture load_texture(std::string_view rpath);

/**
 * Load multiple equally sized textures from a larger texture
 *
 * This function is excellent for loading textures from an atlas or for the purposes of an animation. It loads multiple textures
 * of equal size starting at {xoffset,yoffset}, then moving {w,h} pixels out from that. It does this count times and every time it
 * has done {cols} number of tiles it moves to the next row and continues there.
 *
 * @param rpath The relative file path
 * @param xoffset The starting x coordinate to sample from
 * @param yoffset The starting y coordinate to sample from
 * @param w The width of each texture to extract
 * @param h The height of each texture to extract
 * @param cols The number of columns
 * @param count The total number of sprites/textures to extract
 * @return Vector of data about the loaded textures, in order
 */
std::vector<LoadedTexture> load_texture_partitioned(const char* fp, int xoffset, int yoffset, int w, int h, int cols, int count);

/**
 * Set up a default OpenGL Debug callback for error handling
 */
void create_debug_callback();

/**
 * \todo
 */
template<typename Container>
constexpr typename Container::size_type size_bytes(const Container& container)
{
    return container.size() * sizeof(typename Container::value_type);
}

template<typename... Ts>
std::vector<uint8_t> combine_buffers(const std::vector<Ts>&... vecs)
{
    static_assert(sizeof...(vecs) > 1, "must combine at least two vectors");

    uint32_t offset = 0u;
    std::vector<uint8_t> out((0u + ... + size_bytes(vecs)));

    auto append = [&](auto& vec) {
        std::memcpy(out.data() + offset, vec.data(), size_bytes(vec));
        offset += size_bytes(vec);
    };

    (append(vecs), ...);

    return out;
}

} // namespace cs::gfx
