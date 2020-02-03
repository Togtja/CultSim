#pragma once

#include "render_data.h"

#include <string_view>
#include <vector>

namespace cs
{
namespace gfx
{
class SpriteRenderer
{
private:
    uint32_t m_vao{};

    /** Template VBO */
    uint32_t m_vbo{};

    /** Instance VBO */
    uint32_t m_ivbo{};

    uint32_t m_shader{};

    /** Pointer to GPU memory where the instance variables are */
    SpriteInstanceVertex* m_instance_data{};

    /** Texture handles for color maps */
    std::vector<uint32_t> m_color_texture_handles{};

    /** Texture handles for normal maps */
    std::vector<uint32_t> m_normal_texture_handles{};

    /** Current number of sprites about to be drawn */
    uint32_t m_nsprites = 0u;

    /** Next available texture ID for sprites */
    SpriteTextureID m_next_texture_id{};

public:
    SpriteRenderer();

    void clear();

    /**
     *
     */
    void draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex);

    void display();

    SpriteTextureID get_texture(std::string_view rpath);
};

} // namespace gfx
} // namespace cs
