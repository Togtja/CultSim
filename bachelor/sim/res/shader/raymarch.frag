#version 450 core

#define MAX_DISTANCE 22.0

/** Output color */
layout(location = 0) out vec4 out_color;

/** Number of spheres */
layout(location = 6) uniform float u_sphere_count = 0;

layout(binding = 3, std430) readonly buffer Spheres
{
    vec4 u_sphere_pos[];
};

/** Matrices */
layout(binding = 0, std140) uniform Matrices
{
    mat4 u_view;
    mat4 u_projection;
};

/** Program info */
layout(binding = 2, std140) uniform ProgramInfo
{
    vec2 resolution;
    vec2 cursor_position;
    float runtime;
};

/** Sunlight environment */
layout(binding = 5, std140) uniform Environment
{
    /** Intensities in A */
    vec4 sun_color;
    vec4 ambient_color;

    vec3 sun_direction;
} env;

/** Produce sdf for entire scene */
float map_scene(in vec3 pos)
{
    float d = ~0u;
    for(int i = 0; i < int(u_sphere_count); ++i)
    {
        d = min(d, length(pos - u_sphere_pos[i].xyz * 0.00555) - u_sphere_pos[i].w);
    }

    float ground = pos.z  + 0.1f;
    return min(ground, d);
}

/** Compute normal gradients */
vec3 fake_normals(in vec3 pos)
{
    const vec2 e = vec2(0.1, 0.0);
    return normalize(vec3(map_scene(pos + e.xyy) - map_scene(pos - e.xyy),
                          map_scene(pos + e.yxy) - map_scene(pos - e.yxy),
                          map_scene(pos + e.yyx) - map_scene(pos - e.yyx)));
}

/** Cast a ray from an origin in a given direction */
float cast_ray(in vec3 ro, in vec3 rd)
{   
    float t = 0.f;
    for(int i = 0; i < 30; ++i)
    {
        vec3 pos = ro + rd * t;

        float h = map_scene(pos);
        if(h < 0.002) break;

        t += h;
        if(t > MAX_DISTANCE) break;    
    }
    
    if(t > MAX_DISTANCE) t = -1.f;    
    return t;
}

void main()
{
    vec2 p = (2 * gl_FragCoord.xy - resolution) / resolution.y;

    /** Ray origin and direction, use the camera position */
    vec3 ro = vec3(u_view[3].xy / resolution.x * 8.0, u_view[3].z);
    vec3 rd = normalize(vec3(p, -.5));

    /** Sky default bg color */
    vec3 col = vec3(0.4, 0.75, 1.0) - 0.7 * rd.y;
    col = mix(col, vec3(0.7, 0.75, 0.8), exp(-10.0 * rd.y));

    float t = cast_ray(ro, rd);
    if(t > 0.0)
    {
        vec3 pos = ro + rd * t;
        vec3 nor = fake_normals(pos);

        vec3 material = vec3(0.18);

        float sun_diffuse = clamp(dot(nor, env.sun_direction), 0.0, 1.0);
        float sky_diffuse = clamp(0.5 + 0.5 * dot(nor, vec3(0.f, 1.f, 0.f)), 0.0, 1.0);
        float sun_shadow = step(cast_ray(pos + nor * 0.01, env.sun_direction), 0.0);
        
        col =  material * env.sun_color.rgb * sun_diffuse * sun_shadow;
        col += material * vec3(0.5, 0.8, 0.9) * sky_diffuse;
    }

    col = pow(col, vec3(0.4545));
    out_color = vec4(col, 1.f);
}
