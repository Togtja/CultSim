#version 450 core

/** Per Vertex Attributes */
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;

/** Per Instance Attributes */
layout(location = 2) in vec3 a_offset;
layout(location = 3) in vec3 a_color;
layout(location = 4) in uint a_texture; // TODO: Make it 16 bit if we can

/** Outputs */
layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_texcoord;
layout(location = 2) out uint out_texture;

void main()
{
    out_color    = a_color;
    out_texcoord = a_texcoord;
    out_texture  = a_texture;

    gl_Position = vec4(aPosition, 0.f, 1.f);
}
