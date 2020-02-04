# Create a testing file, to run all the tests

# Define executable
set(EXEC_NAME sim_tests)
add_executable(${EXEC_NAME})

# Add Source Files
target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

# Set Include Directories
target_include_directories(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

# Link with some libraries
target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    $<$<PLATFORM_ID:Linux>:dl>
    cultlib::cultlib
)
