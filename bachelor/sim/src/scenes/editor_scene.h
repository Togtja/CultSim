#pragma once

#include "debug/locale_editor.h"
#include "scene.h"

namespace cs
{
/**
 * Development editor to change files or other conveniet features for development
 *
 * Currently in a very rudimentary state and not very used. Was a good idea, but not a priority yet.
 */
class EditorScene : public IScene
{
private:
    /** Locale editor used to edit strings in various languages */
    LocaleEditor m_locale_editor{};

public:
    void on_enter() override;

    void on_exit() override;

    bool update(float dt) override;

    bool draw() override;
};
} // namespace cs
