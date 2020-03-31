#include "gui_renderer.h"

#include <Ultralight/Ultralight.h>

namespace ul = ultralight;

namespace cs::gfx
{
GuiRenderer::GuiRenderer()
{
    ul::Config config{};
    config.user_agent = "Cultsim";

    ul::Platform::instance().set_config(config);
    ul::Platform::instance().set_gpu_driver(this);
    ul::Platform::instance().set_file_system(&m_filesystem);

    m_renderer = ul::Renderer::Create();

    m_hello_view = m_renderer->CreateView(500, 250, false);
    m_hello_view->LoadHTML("<h1>Hello Cultsim!</h1>");
}

} // namespace cs::gfx
