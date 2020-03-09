#version 450 core

/** Input Attributes */
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;

/** Per Instance Attributes */
layout(location = 2) in vec3 a_offset;
layout(location = 3) in vec3 a_scale;

/** Outputs */
layout(location = 0) out vec3 vs_color;

/** Uniforms */
layout(location = 0) uniform mat4 u_projection;
// layout(location = 1) uniform vec3 u_color;

void main()
{
    vs_color    = a_color;
    gl_Position = u_projection * (vec4((a_position * a_scale) + a_offset, 1.f));
}
