#version 450 core

/** Inputs */
layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in flat uint in_texture;

/** Outputs */
layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(in_color, 1.f);
}
