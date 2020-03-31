# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/filesystem.h
        ${CMAKE_CURRENT_LIST_DIR}/filesystem.cpp
        ${CMAKE_CURRENT_LIST_DIR}/filesystem_test.cpp

        ${CMAKE_CURRENT_LIST_DIR}/gui_filesystem.h
        ${CMAKE_CURRENT_LIST_DIR}/gui_filesystem.cpp
)
