#version 450 core

/** Input Attributes */
layout(location = 0) in vec2 a_position;

/** Outputs */
layout(location = 0) out vec3 vs_color;

/** Uniforms */
layout(location = 0) uniform mat4 u_projection;
layout(location = 1) uniform vec3 u_color;

void main()
{
    vs_color    = u_color;
    gl_Position = u_projection * (vec4(a_position * 5.f, 0.f, 1.f));
}
