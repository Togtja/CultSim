# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/input_handler.h
        ${CMAKE_CURRENT_LIST_DIR}/input_handler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/input_handler_test.cpp
)
