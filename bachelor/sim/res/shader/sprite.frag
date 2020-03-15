#version 450 core

/** Inputs */
layout(location = 0) in VertexData
{
    vec3 color;
    vec2 texcoord;
    flat uint texture;
} vs_in;

/** Outputs */
layout(location = 0) out vec4 out_color;

/** Uniforms */
layout(binding = 0) uniform sampler2DArray colorTextures[8];
layout(binding = 8) uniform sampler2DArray normalTextures[8];

const vec3 light_dir = normalize(vec3(1.f, 1.f, 1.f));

/**
 * Sample a sprite based on the texture data from the vertex shader
 */
vec4 sample_sprite()
{
    return texture(colorTextures[bitfieldExtract(vs_in.texture, 0, 4)],
                   vec3(vs_in.texcoord, bitfieldExtract(vs_in.texture, 4, 5)));
}

/**
 * Sample a normal based on the texture data from the vertex shader
 */
vec4 sample_normal()
{
    return texture(normalTextures[bitfieldExtract(vs_in.texture, 0, 4)],
                   vec3(vs_in.texcoord, bitfieldExtract(vs_in.texture, 4, 5)));
}

void main()
{
    vec3 normal = sample_normal().rgb;
    float effect = dot(normal, light_dir);

    vec3 hover_color  = bitfieldExtract(vs_in.texture, 14, 1) * vec3(1.f, 0.5f, 0.f);
    vec3 select_color = bitfieldExtract(vs_in.texture, 15, 1) * vec3(0.f, 0.5f, 1.f);

    vec3 modifier_color = hover_color + select_color;
    out_color           = sample_sprite() * vec4(vs_in.color, 1.f) * effect + vec4(modifier_color, 0.f);
}
