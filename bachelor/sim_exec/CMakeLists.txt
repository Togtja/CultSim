# Create Excecutable
find_package(OpenGL REQUIRED)

# Program Variables
set(TEAM_NAME g107)

# Define executable
set(EXEC_NAME cultsim)

add_executable(${EXEC_NAME})

target_include_directories(
    ${EXEC_NAME}
    PRIVATE
    ${PHYSFS_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
)

# Link with some libraries
target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    cultlib::cultlib
    $<$<PLATFORM_ID:Linux>:dl>
    OpenGL::GL
    SDL2main
    SDL2
    glm
    EnTT::EnTT
    glad::glad
    ${PHYSFS_LIBRARY}
    sol2::sol2
    ${LUA_LIBRARIES}
    spdlog::spdlog
)
target_sources(
    ${EXEC_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)
# Zip Resource folder and copy to binary directory so we can mount it with PhysFS
add_custom_target(
    cultsim_resource_copy ALL
    COMMAND ${CMAKE_COMMAND} -E tar cv ${CMAKE_CURRENT_BINARY_DIR}/data.zip --format=zip -- shader config.lua l10n
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/res
    DEPENDS cultsim
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/data.zip
    COMMENT "Archiving resource files..."
)
