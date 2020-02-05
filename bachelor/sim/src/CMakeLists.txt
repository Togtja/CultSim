# Add source files for this directory

target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp

    ${CMAKE_CURRENT_LIST_DIR}/application.h
    ${CMAKE_CURRENT_LIST_DIR}/application.cpp
)

# Configure file with CMake Variables
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/constants.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/constants.h
)
