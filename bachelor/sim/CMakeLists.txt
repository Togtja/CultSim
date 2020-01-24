# Create Test Executable to Ensure OpenGL is Working

# Find Libraries to Link With
find_package(OpenGL REQUIRED)
find_package(Lua REQUIRED)

# Define executable
set(EXEC_NAME cultsim)
add_executable(${EXEC_NAME})

# Add Source Files
target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

# Set Include Directories
target_include_directories(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${LUA_INCLUDE_DIR}
)

# Link with some libraries
target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    OpenGL::GL
    $<$<PLATFORM_ID:Linux>:dl>
    glad::glad
    physfs
    SDL2main
    SDL2-static
    ${LUA_LIBRARIES}
    sol2::sol2
    EnTT::EnTT
    spdlog::spdlog
)
