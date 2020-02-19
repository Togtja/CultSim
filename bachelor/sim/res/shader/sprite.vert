#version 450 core

/** Hard Coded Vertex Attributes */
const vec3 positions[6] = {
vec3(-0.5f, -0.5f, 0.f),
vec3(0.5f, -0.5f, 0.f),
vec3(0.5f, 0.5f, 0.f),
vec3(0.5f, 0.5f, 0.f),
vec3(-0.5f, 0.5f, 0.f),
vec3(-0.5f, -0.5f, 0.f)
};

const vec2 tex_coords[6] = {
vec2(0.f, 0.f),
vec2(1.f, 0.f),
vec2(1.f, 1.f),
vec2(1.f, 1.f),
vec2(0.f, 1.f),
vec2(0.f, 0.f)
};

/** Per Instance Attributes */
layout(location = 0) in vec3 a_offset;
layout(location = 1) in vec3 a_color;
layout(location = 2) in uint a_texture;// TODO: Make it 16 bit if we can (and messure)

/** Outputs */
layout(location = 0) out VertexData
{
    vec3 color;
    vec2 texcoord;
    uint texture;
} vs_out;

/** Uniforms */
layout(push_constant) uniform PerFrame
{
    mat4 u_projection;
};

void main()
{
    vs_out.color    = vec3(1.f);
    vs_out.texcoord = tex_coords[gl_VertexIndex];
    vs_out.texture  = 2u;

    gl_Position = u_projection * (vec4(positions[gl_VertexIndex], 1.f) * 10.f + vec4(a_offset, 0.f));
}
