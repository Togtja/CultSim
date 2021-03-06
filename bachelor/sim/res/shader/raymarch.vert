#version 450 core

/** Vertex data, fulscreen triangle */
const vec4 vertices[3] = { 
    vec4(-1.f, 1.f, 0.f, 1.f),
    vec4(3.f, 1.f, 0.f, 1.f),
    vec4(-1.f, -3.f, 0.f, 1.f)
};

void main()
{
    gl_Position = vertices[gl_VertexID];
}
