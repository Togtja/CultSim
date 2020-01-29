GLuint compile_shader(std::string_view source, GLenum type)
{
    /* Must extract c-string to comply with OpenGL interface */
    const char* source_cstr = source.data();
    GLuint shader           = glCreateShader(type);

    glShaderSource(shader, 1, &source_cstr, nullptr);
    glCompileShader(shader);

    /* Check for erros */
    int err, len;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &err);

    if (!err)
    {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        auto log = std::string(len, '\0');

        glGetShaderInfoLog(shader, len, &len, log.data());
        spdlog::error("{} shader compile error: %s\n", get_gl_shader_type_name(type).c_str(), log);

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
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
