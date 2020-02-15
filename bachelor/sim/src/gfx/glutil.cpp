#include "glutil.h"
#include "filesystem/filesystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <spdlog/spdlog.h>

namespace cs::gfx
{
LoadedTexture load_texture(std::string_view rpath)
{
    if (!fs::exists(rpath))
    {
        spdlog::warn("texture does not exist: {}", rpath);
        return {};
    }

    /** Load bytes and parse as image */
    int c{};
    auto out         = LoadedTexture{};
    const auto bytes = fs::read_byte_file(rpath);
    auto pixels      = stbi_load_from_memory(bytes.data(), size_bytes(bytes), &out.width, &out.height, &c, STBI_rgb_alpha);

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
} // namespace cs::gfx
