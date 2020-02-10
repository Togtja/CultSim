# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE

        ${CMAKE_CURRENT_LIST_DIR}/scene.h

        ${CMAKE_CURRENT_LIST_DIR}/mock_scene.h

        ${CMAKE_CURRENT_LIST_DIR}/scene_manager.h
        ${CMAKE_CURRENT_LIST_DIR}/scene_manager.cpp
        ${CMAKE_CURRENT_LIST_DIR}/scene_manager_test.cpp

        ${CMAKE_CURRENT_LIST_DIR}/scenario_scene.h
        ${CMAKE_CURRENT_LIST_DIR}/scenario_scene.cpp
)
