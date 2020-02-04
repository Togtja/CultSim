# Create Excecutable

# Program Variables
set(TEAM_NAME g107)

# Define executable
set(EXEC_NAME cultsim)

add_executable(${EXEC_NAME})

# Link with some libraries
target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    cultlib::cultlib
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
