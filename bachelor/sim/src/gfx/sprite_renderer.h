#pragma once

#include "render_data.h"
#include "camera.h"

#include <string_view>
#include <vector>

namespace cs::gfx
{
class SpriteRenderer
{
private:
    Camera& m_camera;

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
    explicit SpriteRenderer(Camera& camera);

    /**
     * Add a new sprite to the draw list using the given position color and texture
     *
     * @param pos The position to draw the sprite at in world coordinates
     * @param color The color tint to apply to the sprite texture
     * @param tex The texture ID of the texture that you want to draw
     */
    void draw(glm::vec3 pos, glm::vec3 color, SpriteTextureID tex);

    /**
     * Display submits the draw list for this frame to the GPU and renders the scene
     */
    void display();

    /**
     * Get the texture id of a given texture
     *
     * @param rpath Relative path to the texture file you want the ID of
     * @return An ID to the given texture
     */
    SpriteTextureID get_texture(std::string_view rpath);

private:
    bool increment_next_texture_id();

    void init_vbo_and_vao();

    void init_shader();

    void init_texture_slots();
};

} // namespace cs
