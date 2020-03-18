#version 450 core

/** Input Attributes */
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;

/** Per Instance Attributes */
layout(location = 2) in vec3 a_offset;
layout(location = 3) in vec3 a_scale;
layout(location = 4) in vec3 a_icolor;

/** Outputs */
layout(location = 0) out vec3 vs_color;

/** Uniforms */
layout(binding = 0, std140) uniform Matrices
{
    mat4 u_projection;
};

layout(location = 1) uniform float u_instance_factor;

void main()
{
    vs_color               = mix(a_icolor, a_color, u_instance_factor);
    const vec3 real_scale  = mix(a_scale, vec3(1.f), u_instance_factor);
    const vec3 real_offset = mix(a_offset, vec3(0.f), u_instance_factor);
    gl_Position            = u_projection * (vec4((a_position * real_scale) + real_offset, 1.f));
}
