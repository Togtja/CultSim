# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/events.h

        ${CMAKE_CURRENT_LIST_DIR}/reflection.h
        ${CMAKE_CURRENT_LIST_DIR}/reflection.cpp

        ${CMAKE_CURRENT_LIST_DIR}/scenario.h
        ${CMAKE_CURRENT_LIST_DIR}/scenario.cpp
)
