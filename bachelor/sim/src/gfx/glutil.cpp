std::string get_gl_shader_type_name(GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER: return "vertex"; break;
        case GL_GEOMETRY_SHADER: return "geometry"; break;
        case GL_TESS_CONTROL_SHADER: return "tesselation control"; break;
        case GL_TESS_EVALUATION_SHADER: return "tesselation evaluation"; break;
        case GL_FRAGMENT_SHADER: return "fragment"; break;
        case GL_COMPUTE_SHADER: return "compute"; break;
        default: return "invalid"; break;
    }
}
