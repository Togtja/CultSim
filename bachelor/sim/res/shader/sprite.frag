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

/** Available materials */
struct Material
{
    float diffuse;
    float specular;
    float gloss;
    float emissive;
};

layout(binding = 1, std140) uniform Materials
{
    Material[8] materials;
};

/** Sunlight environment */
layout(binding = 5, std140) uniform Environment
{
    /** Intensities in A */
    vec4 sun_color;
    vec4 ambient_color;

    vec3 sun_direction;
} env;

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
    /* Sample normal / color textures */
    const vec4 diffuse = sample_sprite();
    const vec3 normal = sample_normal().rgb * 2.0 - 1.0;
    const uint material_id = bitfieldExtract(vs_in.texture, 13, 3);

    /* Rotate light to illuminate sprite from correct direction based on normals */
    const vec3 rotated_light = normalize(vec3(vs_in.light_mat * env.sun_direction.xy, env.sun_direction.z));
    const vec3 specular_direction = normalize(vec3(0, 0, 1) + rotated_light);

    /* Compute emissive component */
    const vec3 e_coef = materials[material_id].emissive * vs_in.color;

    /* Compute Diffuse / Spec */
    float d_coef = max(dot(normal, rotated_light), 0.0) * materials[material_id].diffuse * env.sun_color.a;
    float s_coef = max(dot(normal, specular_direction), 0.0);
    s_coef = pow(s_coef, materials[material_id].gloss) * materials[material_id].specular;

    const vec3 diffuse_color = diffuse.rgb * vs_in.color;
    out_color = vec4(diffuse_color * env.sun_color.rgb * d_coef +
                     diffuse_color * env.ambient_color.rgb * env.ambient_color.a +
                     s_coef + e_coef,
                     diffuse.a);
}
