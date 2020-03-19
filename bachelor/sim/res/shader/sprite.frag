#version 450 core

/** Inputs */
layout(location = 0) in VertexData
{
    vec3 color;
    vec2 texcoord;
    flat uint texture;
    mat2 light_mat;
} vs_in;

/** Outputs */
layout(location = 0) out vec4 out_color;

/** Uniforms */
layout(binding = 0) uniform sampler2DArray colorTextures[8];
layout(binding = 8) uniform sampler2DArray normalTextures[8];
layout(location = 1) uniform vec3 light_dir = vec3(1.f, 1.f, 1.f);
layout(location = 2) uniform vec3 ambient_col = vec3(0.05f, 0.05f, 0.1f);

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
    /**  Hover and selection */
    const vec3 hover_color  = bitfieldExtract(vs_in.texture, 14, 1) * vec3(1.f, 0.5f, 0.f);
    const vec3 select_color = bitfieldExtract(vs_in.texture, 15, 1) * vec3(0.f, 0.5f, 1.f);

    const vec4 diffuse = sample_sprite();
    const vec3 normal = sample_normal().rgb * 2.0 - 1.0;

    const vec3 rotated_light = normalize(vec3(vs_in.light_mat * light_dir.xy, light_dir.z));

    float d_coef = max(dot(normal, rotated_light), 0.0);

    vec3 specular_direction = normalize(vec3(0, 0, 1) + rotated_light);
    float s_coef = max(dot(normal, specular_direction), 0.0);
    s_coef = pow(s_coef, 10.0) * bitfieldExtract(vs_in.texture, 13, 1);

    vec3 modifier_color = hover_color + select_color;
    out_color = vec4(diffuse.rgb * vs_in.color * d_coef + ambient_col + s_coef * 0.25f + vec3(modifier_color), diffuse.a);
}
