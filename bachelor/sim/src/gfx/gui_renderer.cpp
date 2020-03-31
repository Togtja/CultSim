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
}

} // namespace cs::gfx
