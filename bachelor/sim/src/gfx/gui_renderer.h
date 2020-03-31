#pragma once

#include "filesystem/gui_filesystem.h"

#include <Ultralight/platform/GPUDriver.h>

namespace cs::gfx
{
class GuiRenderer : public ultralight::GPUDriver
{
private:
    fs::GuiFilesystem m_filesystem{};

public:
    GuiRenderer();
};
} // namespace cs::gfx
