# Create a testing file, to run all the tests

# Define executable
set(EXEC_NAME trivtest)
add_executable(${EXEC_NAME})

# Add Source Files
target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_compile_features(
    ${EXEC_NAME}
    PUBLIC
    cxx_std_17
)

# Link with some libraries
target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    trivlib
    $<$<PLATFORM_ID:Linux>:dl>
)
