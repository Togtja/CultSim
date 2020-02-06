# Add source files for this directory

target_sources(
    ${EXEC_NAME}
    PRIVATE

    ${CMAKE_CURRENT_LIST_DIR}/scene.h

    ${CMAKE_CURRENT_LIST_DIR}/scenemanager.h
    ${CMAKE_CURRENT_LIST_DIR}/scenemanager.cpp
)
