# Create Test Executable to Ensure OpenGL is Working

# Find Libraries to Link With
find_package(OpenGL REQUIRED)
find_package(Lua REQUIRED)
find_package(PhysFS REQUIRED)

# Program Variables
set(TEAM_NAME g107)

# Define executable
set(EXEC_NAME cultlib)

add_library(${EXEC_NAME} SHARED)
add_library(${EXEC_NAME}::${EXEC_NAME} ALIAS ${EXEC_NAME})

# Add Source Files
include(${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/gfx/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/gfx/ImGUI/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/l10n/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/include/cultsim/CMakeLists.txt)


# Set Include Directories
target_include_directories(
    ${EXEC_NAME}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include

    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include/cultsim
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
)

# Link with some libraries
target_link_libraries(
    ${EXEC_NAME}
    PUBLIC
    doctest

    PRIVATE
    OpenGL::GL
    $<$<PLATFORM_ID:Linux>:dl>
    glad::glad
    ${PHYSFS_LIBRARY}
    SDL2main
    SDL2
    ${LUA_LIBRARIES}
    sol2::sol2
    EnTT::EnTT
    spdlog::spdlog
    glm::glm
)
