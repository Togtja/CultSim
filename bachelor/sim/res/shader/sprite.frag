#version 450 core

/** Inputs */
layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in flat uint in_texture;

/** Outputs */
layout(location = 0) out vec4 out_color;

/** Uniforms */
layout(location = 0, binding = 0) uniform sampler2DArray colorTextures[8];
layout(location = 1, binding = 8) uniform sampler2DArray normalTextures[8];

void main()
{
    out_color = vec4(in_color, 1.f);
}
