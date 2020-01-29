# Add source files for this directory

target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/render_data.h

    ${CMAKE_CURRENT_LIST_DIR}/glutil.h
    ${CMAKE_CURRENT_LIST_DIR}/glutil.cpp

    ${CMAKE_CURRENT_LIST_DIR}/sprite_renderer.h
    ${CMAKE_CURRENT_LIST_DIR}/sprite_renderer.cpp

)
