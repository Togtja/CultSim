#include "editor_scene.h"
#include "common_helpers.h"
#include "input/input_handler.h"

namespace cs
{
void EditorScene::on_enter()
{
    m_locale_editor.init(m_context->lua_state);
    input::get_input().add_context(input::EKeyContext::EditorScene, true);
}

void EditorScene::on_exit()
{
    input::get_input().remove_context(input::EKeyContext::EditorScene);
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
