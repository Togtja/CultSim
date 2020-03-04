# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/parsers.h
        ${CMAKE_CURRENT_LIST_DIR}/parsers.cpp
)
