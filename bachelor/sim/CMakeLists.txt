# Create Test Executable to Ensure OpenGL is Working

# Find Libraries to Link With
find_package(OpenGL REQUIRED)
find_package(Lua REQUIRED)
find_package(PhysFS REQUIRED)

# Program Variables
set(TEAM_NAME g107)

# Define executable
set(EXEC_NAME cultsim)
add_executable(${EXEC_NAME})

# Add Source Files
include(${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/gfx/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/gfx/ImGUI/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/l10n/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/debug/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/entity/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/entity/components/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/entity/systems/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/entity/actions/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/filesystem/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/input/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/scenes/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/src/ai/CMakeLists.txt)


# Discover tests written by the executable
doctest_discover_tests(${EXEC_NAME} EXTRA_ARGS --exit)

# Set Include Directories
target_include_directories(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${LUA_INCLUDE_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link with some libraries
target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    OpenGL::GL
    $<$<PLATFORM_ID:Linux>:dl>
    glad::glad
    ${PHYSFS_LIBRARY}
    SDL2main
    SDL2-static
    ${LUA_LIBRARIES}
    sol2::sol2
    EnTT::EnTT
    spdlog::spdlog
    glm::glm
    doctest
    $<$<CXX_COMPILER_ID:GNU>:tbb>

    # Link with gcov on GNU for code coverage
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:gcov>
)

# Explicitly enable C++ 17 for this executable
target_compile_features(
        ${EXEC_NAME}
        PRIVATE
        cxx_std_17
)

# Set compile flags for code coverage computation on Clang / GNU
target_compile_options(
        ${EXEC_NAME}
        PRIVATE
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:Clang>>:-fprofile-instr-generate -fcoverage-mapping>
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU>>:--coverage>
)

# Zip Resource folder and copy to binary directory so we can mount it with PhysFS
add_custom_target(
        cultsim_resource_copy ALL
        COMMAND ${CMAKE_COMMAND} -E tar cv ${CMAKE_CURRENT_BINARY_DIR}/data.zip --format=zip -- shader config.lua l10n sprites fonts script keybinding.lua
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/res
        DEPENDS cultsim
        BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/data.zip
        COMMENT "Archiving resource files..."
)
