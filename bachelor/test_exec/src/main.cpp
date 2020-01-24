#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <vector>

#include<fstream>
#include<string>

#include <SDL.h>
#include <glad/glad.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

// Vertex shader source code
const char* vertexSource = "#version 450 core\n"
                           "layout(location=0) in vec2 aPosition;\n"
                           "layout(location=0) uniform float uTime = 0.f;\n"
                           "layout(location=1) uniform vec3 uColor = vec3(0.8f, 0.3f, 0.0f);\n"
                           "layout(location=0) out vec3 vs_col;\n"
                           "void main() {"
                           "      mat2 rotMatrix = mat2(cos(uTime), -sin(uTime), sin(uTime), cos(uTime));"
                           "      gl_Position = vec4(rotMatrix * aPosition, 0.f, 1.f);"
                           "      vs_col = uColor;"
                           "}";

// Fragment shader source code
const char* fragSource = "#version 450 core\n"
                         "out vec4 color;\n"
                         "layout(location=0) in vec3 vs_col;\n"
                         "void main(){"
                         "    color = vec4(vs_col, 1.f);"
                         "}";

const std::string readGLSLfromFile(const char* relativePath)
{
    std::ifstream glsl (relativePath, std::ios::in);
    if(!glsl.is_open())
    {
        //Creating an error message
        std::string errMsg = "failed to find " + std::string(relativePath) + "\n";
        fprintf(stdout, errMsg.c_str());
        return "";
    }

    //reading the file
    std::string content;
    std::string line;
    while(std::getline(glsl, line))
    {
        content += (line + "\n");
    }
    glsl.close();
    return content;
}

/*!
 * \brief The vec2 struct contains two points to represent a 2D float vector
 */
struct vec2
{
    float x, y;
};

/*!
 * \brief The vec2i struct contains two points to represent a 2D integer vector
 */
struct vec2i
{
    int x, y;
};

/*!
 * \brief The GraphicsContext struct contains all required elements to maintain
 * an SDL2 OpenGL rendering context for this example program.
 */
struct GraphicsContext
{
    // The SDL Window
    SDL_Window* window;

    // The OpenGL Context
    SDL_GLContext context;

    // The active Vertex Array Object
    GLuint vao;

    // Active Shader Program
    GLuint program;

    // Number of rendered frames
    GLuint frameCount;
} gfxContext = {NULL, NULL, 0, 0, 0};

/*!
 * \brief compileShader
 * \param source
 * \param type is the type of the shader, accepts GL_VERTEX_SHADER or GL_FRAGMENT_SHADER (should accept more)
 * \return OpenGL name of the newly created shader
 */
GLuint compileShader(const char* source, GLenum type)
{
    // Only allow vertex/fragment shaders for now
    assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);

    // Create shader, attach source code and compile it
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for erros
    int err;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &err);

    if (!err)
    {
        return 0;
    }

    return shader;
}

/*!
 * \brief createProgram will create and link a program from the two supplied shaders
 * \param sz is the number of elements in the shaders array
 * \param shaders is an array of shaders that shall be linked and attached to the resulting program
 * \return OpenGL name of the newly created program
 */
GLuint createProgram(int sz, GLuint* shaders)
{
    // Create program
    GLuint program = glCreateProgram();

    // Attach all shaders that should be part of the program
    for (int i = 0; i < sz; ++i)
    {
        glAttachShader(program, shaders[i]);
    }

    // Link the program
    glLinkProgram(program);

    // Check for errors
    int err;
    glGetProgramiv(program, GL_LINK_STATUS, &err);

    if (!err)
    {
        return 0;
    }

    return program;
}

/*!
 * \brief createWindow will create a Window with the provided title at the provided size
 * \param title is the title of the window
 * \param size is the requested size (and resolution) of the window
 * \return A pointer to the newly created SDL Window, or NULL if it failed
 */
SDL_Window* createWindow(const char* title, struct vec2i* size)
{
    SDL_Window* window = SDL_CreateWindow(title,
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          size->x,
                                          size->y,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window)
    {
        fprintf(stderr, "Failed to create SDL2 Window\n");
        return NULL;
    }

    return window;
}

/*!
 * \brief createContext will create an SDL2 OpenGL profile with the provided major and minor version, using
 * the Core profile if the cp flag is 1, and in compability if it is 0
 * \param window is the window that this Context should show in
 * \param major is the major OpenGL Version
 * \param minor is the minor OpenGL Version
 * \param pc is a flag (0/1) that if true means a Core Profile Context shall be requested
 * \return A handle to an SDL OpenGL Context
 */
SDL_GLContext createContext(SDL_Window* window, int major, int minor, short cp)
{
    SDL_GLprofile profile = (cp ? SDL_GL_CONTEXT_PROFILE_CORE : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

    // Create a double buffer context with the provided settings
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    return SDL_GL_CreateContext(window);
}

/*!
 * \brief initGL intiializes all OpenGL contexts, creates a window and compiles
 * and links the shaders necessary to run this program.
 * \param window_size is the size of the window to create
 */
void initGL(struct vec2i* window_size)
{
    // Attempt to initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf(stderr, "Failed to Initialize SDL2\n");
        exit(EXIT_FAILURE);
    }

    // Create Window
    gfxContext.window = createWindow("Core Profile", window_size);

    // Create OpenGL Context
    gfxContext.context = createContext(gfxContext.window, 4, 5, 1);

    // Enable V-Sync
    SDL_GL_SetSwapInterval(1);

    // Load OpenGL Functions
    if (!gladLoadGL())
    {
        fprintf(stderr, "Glad failed to load GL 4.5 Core Profile functions\n");
        exit(EXIT_FAILURE);
    }

    // Create shaders and program
    GLuint shaders[2];
    std::string vertex = readGLSLfromFile("bachelor/test_exec/src/vertex.vert");
    std::string fragment = readGLSLfromFile("bachelor/test_exec/src/fragement.frag");
    if(vertex == "" || fragment == "")
    {
        fprintf(stderr, "Failed to Initialize the vertex or the fragment from file\n");
        exit(EXIT_FAILURE);
    }
    shaders[0] = compileShader(vertex.c_str(), GL_VERTEX_SHADER);
    shaders[1] = compileShader(fragment.c_str(), GL_FRAGMENT_SHADER);

    gfxContext.program = createProgram(2, shaders);

    // It is safe to delete shaders even though a program uses them as if they are in use they
    // will only be flagged for deletion, and not deleted until the program is delete
    glDeleteShader(shaders[0]);
    glDeleteShader(shaders[1]);
}

/*!
 * \brief terminateGL cleans up the GL Context and SDL window created by initGL
 */
void terminateGL()
{
    SDL_DestroyWindow(gfxContext.window);
    SDL_Quit();
}

/* Clever Class */
class VertexBuffer
{
private:
    GLuint vbo = 0u;

public:
    explicit VertexBuffer(const std::vector<vec2>& data)
    {
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, data.size() * sizeof(vec2), data.data(), 0);
    }

    VertexBuffer(const VertexBuffer& other) = delete;
    VertexBuffer& operator=(const VertexBuffer& other) = delete;

    ~VertexBuffer() noexcept
    {
        glDeleteBuffers(1, &vbo);
    }

    GLuint get() const
    {
        return vbo;
    }
};

void process(VertexBuffer& vbo)
{
    auto vbo2 = vbo;
}

/* coreProfile: Minimal OpenGL 4.5 Core Profile demo that uses some key ideas of Direct State Access that
 * was introduced to OpenGL in version 4.5. In particular the glCreateBuffers over glGenBuffers,
 * and the glNamedBufferStroage functions that allow for data transfer without binding buffers,
 * and the functionality to assosciate an element buffer with the vertex array.
 */
int main(int argc, char* argv[])
{
    struct vec2i winSize = {WINDOW_WIDTH, WINDOW_HEIGHT};
    initGL(&winSize);

    // Create data required to draw a quad
    std::vector<vec2> vertices         = {{-.5f, -.5f}, {.5f, -.5f}, {.5f, .5f}, {-.5f, .5f}};
    std::vector<unsigned char> indices = {0, 1, 2, 2, 3, 0};

    // Clever Example
    auto vbo = VertexBuffer(vertices);
    process(vbo);

    // Create VBO and IBO buffers respectively
    GLuint buffers[2];
    glCreateBuffers(2, buffers);

    // Fill buffers with data
    glNamedBufferStorage(buffers[1], sizeof(unsigned char) * indices.size(), indices.data(), 0);

    // Create Vertex Array
    glCreateVertexArrays(1, &gfxContext.vao);

    // Assosciate VAO with the two buffers (vertices and indices)
    glVertexArrayVertexBuffer(gfxContext.vao, 0, vbo.get(), 0, sizeof(struct vec2));
    glVertexArrayElementBuffer(gfxContext.vao, buffers[1]);

    // Set attribute 0 to be a 2 component float vector
    glVertexArrayAttribFormat(gfxContext.vao, 0, 2, GL_FLOAT, GL_FALSE, 0);

    // Assosciate attribute 0 with buffer number 0
    glVertexArrayAttribBinding(gfxContext.vao, 0, 0);

    // Enable the attribute
    glEnableVertexArrayAttrib(gfxContext.vao, 0);

    // Bind VAO only in order to draw, then enable the shader program
    glBindVertexArray(gfxContext.vao);
    glUseProgram(gfxContext.program);

    // Set the background clear color
    glClearColor(0.09f, 0.09f, 0.09f, 0.0f);

    int active = 1;
    while (active)
    {
        ++gfxContext.frameCount;

        // Deal with events to close the window when ESCAPE is pressed, or it is closed conventionally
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                active = 0;
            }
            else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            {
                active = 0;
            }
        }

        // Clear framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Pass "time passed" to shader
        glUniform1f(0, SDL_GetTicks() / 500.f);

        // Dynamically change color of quad based on sin and cos of the frame number
        glUniform3f(1,
                    (sinf(gfxContext.frameCount / 1000.f) + 1.f) / 2.f,
                    0.8f,
                    (cosf(gfxContext.frameCount / 2000.f) + 1.f) / 2.f);

        // Draw as Triangles, N indices of type Unsigned Byte
        glDrawElements(GL_TRIANGLES, sizeof(unsigned char) * indices.size(), GL_UNSIGNED_BYTE, NULL);

        // Swap back and front buffer to make image visible
        SDL_GL_SwapWindow(gfxContext.window);
    }

    // Clean up buffers
    glDeleteVertexArrays(1, &gfxContext.vao);
    glDeleteBuffers(2, buffers);

    terminateGL();
    return EXIT_SUCCESS;
}
