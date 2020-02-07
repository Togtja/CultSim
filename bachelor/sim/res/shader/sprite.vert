#version 450 core

/** Per Vertex Attributes */
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texcoord;

/** Per Instance Attributes */
layout(location = 2) in vec3 a_offset;
layout(location = 3) in vec3 a_color;
layout(location = 4) in uint a_texture; // TODO: Make it 16 bit if we can (and messure)

/** Outputs */
layout(location = 0) out VertexData
{
    vec3 color;
    vec2 texcoord;
    uint texture;
} vs_out;

layout(location = 0) uniform mat4 u_projection;

void main()
{
    vs_out.color    = a_color;
    vs_out.texcoord = a_texcoord;
    vs_out.texture  = a_texture;

    gl_Position = u_projection * (vec4(a_position, 1.f) * 10.f + vec4(a_offset, 0.f));
}
