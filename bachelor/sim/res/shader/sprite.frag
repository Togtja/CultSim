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

vec4 sample_sprite()
{
    return texture(colorTextures[bitfieldExtract(vs_in.texture, 0, 4)], vec3(vs_in.texcoord, bitfieldExtract(vs_in.texture, 4, 5)));
}

void main()
{
    out_color = vec4(1.f, 0.5f, 1.f, 1.f);// sample_sprite() * vec4(vs_in.color, 1.f);
}
