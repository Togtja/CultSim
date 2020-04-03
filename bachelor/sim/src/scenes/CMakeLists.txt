# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE

        ${CMAKE_CURRENT_LIST_DIR}/scene.h

        ${CMAKE_CURRENT_LIST_DIR}/mock_scene.h

        ${CMAKE_CURRENT_LIST_DIR}/scene_manager.h
        ${CMAKE_CURRENT_LIST_DIR}/scene_manager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/scene_manager_test.cpp

        ${CMAKE_CURRENT_LIST_DIR}/pausemenu_scene.h
        ${CMAKE_CURRENT_LIST_DIR}/pausemenu_scene.cpp

        ${CMAKE_CURRENT_LIST_DIR}/preference_scene.h
        ${CMAKE_CURRENT_LIST_DIR}/preference_scene.cpp

        ${CMAKE_CURRENT_LIST_DIR}/scenario_scene.h
        ${CMAKE_CURRENT_LIST_DIR}/scenario_scene.cpp

        ${CMAKE_CURRENT_LIST_DIR}/mainmenu_scene.h
        ${CMAKE_CURRENT_LIST_DIR}/mainmenu_scene.cpp

        ${CMAKE_CURRENT_LIST_DIR}/editor_scene.h
        ${CMAKE_CURRENT_LIST_DIR}/editor_scene.cpp

        ${CMAKE_CURRENT_LIST_DIR}/loadsenario_scene.h
        ${CMAKE_CURRENT_LIST_DIR}/loadsenario_scene.cpp
)
