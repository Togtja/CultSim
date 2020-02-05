#version 450 core

/** Inputs */
layout(location = 0) in vec3 vs_color;

/** Outputs */
layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(vs_color, 1.f);
}
