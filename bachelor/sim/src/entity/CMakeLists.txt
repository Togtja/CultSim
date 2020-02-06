# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/components.h

        ${CMAKE_CURRENT_LIST_DIR}/system.h

        ${CMAKE_CURRENT_LIST_DIR}/ai.h
        ${CMAKE_CURRENT_LIST_DIR}/ai.cpp
)
