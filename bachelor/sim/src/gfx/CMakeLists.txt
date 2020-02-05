# Add source files for this directory

target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/render_data.h

    ${CMAKE_CURRENT_LIST_DIR}/glutil.h
    ${CMAKE_CURRENT_LIST_DIR}/glutil.cpp

    ${CMAKE_CURRENT_LIST_DIR}/sprite_renderer.h
    ${CMAKE_CURRENT_LIST_DIR}/sprite_renderer.cpp

    ${CMAKE_CURRENT_LIST_DIR}/vao_builder.h
    ${CMAKE_CURRENT_LIST_DIR}/vao_builder.cpp

    ${CMAKE_CURRENT_LIST_DIR}/debug_renderer.h
    ${CMAKE_CURRENT_LIST_DIR}/debug_renderer.cpp

    ${CMAKE_CURRENT_LIST_DIR}/renderer.h
    ${CMAKE_CURRENT_LIST_DIR}/renderer.cpp

    ${CMAKE_CURRENT_LIST_DIR}/primitives.h
    ${CMAKE_CURRENT_LIST_DIR}/primitives.cpp

    ${CMAKE_CURRENT_LIST_DIR}/camera.h
    ${CMAKE_CURRENT_LIST_DIR}/camera.cpp

    ${CMAKE_CURRENT_LIST_DIR}/window.h
    ${CMAKE_CURRENT_LIST_DIR}/window.cpp
)
