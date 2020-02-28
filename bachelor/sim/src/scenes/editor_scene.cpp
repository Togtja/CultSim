#include "editor_scene.h"
#include <common_helpers.h>

namespace cs
{
void EditorScene::on_enter()
{
    m_locale_editor.init(m_context->lua_state);
}

void EditorScene::on_exit()
{
}

bool EditorScene::update(float dt)
{
    m_locale_editor.show();
    return false;
}

bool EditorScene::draw()
{
    return false;
}
} // namespace cs
