#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

#include <volk.h>

namespace cs::gfx
{
#define VK_CHECK(call)                                                                                                           \
    do                                                                                                                           \
    {                                                                                                                            \
        VkResult result_ = call;                                                                                                 \
        assert(result_ == VK_SUCCESS);                                                                                           \
    } while (false)

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
 * Get the size of a container's contents in bytes
 */
template<typename Container>
constexpr typename Container::size_type size_bytes(const Container& container)
{
    return container.size() * sizeof(typename Container::value_type);
}

} // namespace cs
