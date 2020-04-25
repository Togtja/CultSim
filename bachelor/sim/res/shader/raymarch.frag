#version 450 core

/** Output color */
layout(location = 0) out vec4 out_color;

/** Matrices */
layout(binding = 0, std140) uniform Matrices
{
    mat4 u_projection;
};

/** Sunlight environment */
layout(binding = 5, std140) uniform Environment
{
    /** Intensities in A */
    vec4 sun_color;
    vec4 ambient_color;

    vec3 sun_direction;
} env;

void main()
{
    out_color = vec4(1.f, 0.f, 0.f, 1.f);
}
