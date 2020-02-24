#pragma once

#include "debug/locale_editor.h"
#include "scene.h"

namespace cs
{
class EditorScene : public IScene
{
private:
    LocaleEditor m_locale_editor;

public:
    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
} // namespace cs
