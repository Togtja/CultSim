#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#    define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <gfx/glutil.h>

#include <cstdio>
#include <cstdint>

#include <glad/glad.h>

// OpenGL Data
static GLuint g_FontTexture              = 0, g_VaoHandle{};
static GLuint g_ShaderHandle             = 0;
static constexpr int g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0; // Uniforms location
static constexpr int g_AttribLocationVtxPos = 0, g_AttribLocationVtxUV = 1,
                     g_AttribLocationVtxColor = 2; // Vertex attributes location
static unsigned int g_VboHandle = 0, g_ElementsHandle = 0;

bool ImGui_ImplOpenGL45_Init()
{
    ImGuiIO& io            = ImGui::GetIO();
    io.BackendRendererName = "imgui_impl_opengl4.5";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

    return true;
}

void ImGui_ImplOpenGL3_Shutdown()
{
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
}

void ImGui_ImplOpenGL3_NewFrame()
{
    if (!g_ShaderHandle)
    {
        ImGui_ImplOpenGL3_CreateDeviceObjects();
    }
}

static void ImGui_ImplOpenGL3_SetupRenderState(ImDrawData* draw_data, GLuint vertex_array_object)
{
    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
    glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom
    // right). DisplayPos is (0,0) for single viewport apps.
    float L                            = draw_data->DisplayPos.x;
    float R                            = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T                            = draw_data->DisplayPos.y;
    float B                            = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    const float ortho_projection[4][4] = {
        {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
        {(R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f},
    };
    glUseProgram(g_ShaderHandle);
    glUniform1i(g_AttribLocationTex, 0);
    glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
#ifdef GL_SAMPLER_BINDING
    glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
#endif

    glBindVertexArray(vertex_array_object);

    // Bind vertex/index buffers and setup attributes for ImDrawVert
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
}

// OpenGL3 Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main
// loop) Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state
// explicitly, in order to be able to run within any OpenGL engine that doesn't do so.
void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width  = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

        // Backup GL state
#ifdef GL_POLYGON_MODE
    GLint last_polygon_mode[2];
    glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
    GLint last_scissor_box[4];
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
    bool clip_origin_lower_left        = true;
#if defined(GL_CLIP_ORIGIN) && !defined(__APPLE__)
    GLenum last_clip_origin = 0;
    glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&last_clip_origin); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
    if (last_clip_origin == GL_UPPER_LEFT)
        clip_origin_lower_left = false;
#endif

    ImGui_ImplOpenGL3_SetupRenderState(draw_data, g_VaoHandle);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off   = draw_data->DisplayPos;       // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        // Upload vertex/index buffers
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
                     (const GLvoid*)cmd_list->VtxBuffer.Data,
                     GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
                     (const GLvoid*)cmd_list->IdxBuffer.Data,
                     GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset
                // render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplOpenGL3_SetupRenderState(draw_data, g_VaoHandle);
                else
                    pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
                    // Apply scissor/clipping rectangle
                    if (clip_origin_lower_left)
                        glScissor((int)clip_rect.x,
                                  (int)(fb_height - clip_rect.w),
                                  (int)(clip_rect.z - clip_rect.x),
                                  (int)(clip_rect.w - clip_rect.y));
                    else
                        glScissor((int)clip_rect.x,
                                  (int)clip_rect.y,
                                  (int)clip_rect.z,
                                  (int)clip_rect.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)

                    // Bind texture, Draw
                    glBindTextureUnit(0u, (GLuint)(intptr_t)pcmd->TextureId);
                    glDrawElementsBaseVertex(GL_TRIANGLES,
                                             (GLsizei)pcmd->ElemCount,
                                             sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                                             (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)),
                                             (GLint)pcmd->VtxOffset);
                }
            }
        }
    }

    if (last_enable_scissor_test)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

bool ImGui_ImplOpenGL45_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    glCreateTextures(GL_TEXTURE_2D, 1, &g_FontTexture);
    glTextureParameteri(g_FontTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(g_FontTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTextureStorage2D(g_FontTexture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(g_FontTexture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (ImTextureID)(intptr_t)g_FontTexture;

    return true;
}

void ImGui_ImplOpenGL45_DestroyFontsTexture()
{
    if (g_FontTexture)
    {
        ImGuiIO& io = ImGui::GetIO();
        glDeleteTextures(1, &g_FontTexture);
        io.Fonts->TexID = 0;
        g_FontTexture   = 0;
    }
}

bool ImGui_ImplOpenGL3_CreateDeviceObjects()
{
    const GLchar* vertex_shader_source = "#version 450 core\n"
                                         "layout(location = 0) in vec2 Position;\n"
                                         "layout(location = 1) in vec2 UV;\n"
                                         "layout(location = 2) in vec4 Color;\n"
                                         "layout(location = 0) uniform mat4 ProjMtx;\n"
                                         "layout(location = 0) out vec2 Frag_UV;\n"
                                         "layout(location = 1) out vec4 Frag_Color;\n"
                                         "void main()\n"
                                         "{\n"
                                         "    Frag_UV = UV;\n"
                                         "    Frag_Color = Color;\n"
                                         "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
                                         "}\n";

    const GLchar* fragment_shader_source = "#version 450 core\n"
                                           "layout(location = 0) in vec2 Frag_UV;\n"
                                           "layout(location = 1) in vec4 Frag_Color;\n"
                                           "layout(binding = 0) uniform sampler2D Texture;\n"
                                           "layout (location = 0) out vec4 Out_Color;\n"
                                           "void main()\n"
                                           "{\n"
                                           "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
                                           "}\n";

    // Create shaders
    auto v_shader = cs::gfx::compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
    auto f_shader = cs::gfx::compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

    g_ShaderHandle = cs::gfx::create_program({v_shader, f_shader});
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    // Create buffers
    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    // Create buffers
    glCreateBuffers(1, &g_VboHandle);
    glCreateBuffers(1, &g_ElementsHandle);

    // Create VAO
    glCreateVertexArrays(1, &g_VaoHandle);
    glVertexArrayVertexBuffer(g_VaoHandle, 0, g_VboHandle, 0u, sizeof(ImDrawVert));
    glVertexArrayElementBuffer(g_VaoHandle, g_ElementsHandle);

    glVertexArrayAttribBinding(g_VaoHandle, g_AttribLocationVtxPos, 0);
    glVertexArrayAttribBinding(g_VaoHandle, g_AttribLocationVtxUV, 0);
    glVertexArrayAttribBinding(g_VaoHandle, g_AttribLocationVtxColor, 0);

    glVertexArrayAttribFormat(g_VaoHandle, g_AttribLocationVtxPos, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, pos));
    glVertexArrayAttribFormat(g_VaoHandle, g_AttribLocationVtxUV, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, uv));
    glVertexArrayAttribFormat(g_VaoHandle, g_AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, IM_OFFSETOF(ImDrawVert, col));

    glEnableVertexArrayAttrib(g_VaoHandle, 0);
    glEnableVertexArrayAttrib(g_VaoHandle, 1);
    glEnableVertexArrayAttrib(g_VaoHandle, 2);

    ImGui_ImplOpenGL45_CreateFontsTexture();
    return true;
}

void ImGui_ImplOpenGL3_DestroyDeviceObjects()
{
    if (g_VboHandle)
    {
        glDeleteBuffers(1, &g_VboHandle);
        g_VboHandle = 0;
    }
    if (g_ElementsHandle)
    {
        glDeleteBuffers(1, &g_ElementsHandle);
        g_ElementsHandle = 0;
    }
    if (g_ShaderHandle)
    {
        glDeleteProgram(g_ShaderHandle);
        g_ShaderHandle = 0;
    }
    if (g_VaoHandle)
    {
        glDeleteVertexArrays(1, &g_VaoHandle);
    }

    ImGui_ImplOpenGL45_DestroyFontsTexture();
}
