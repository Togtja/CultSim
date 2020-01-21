# Create Test Executable to Ensure OpenGL is Working

# Find Libraries to Link With
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# Define executable
set(EXEC_NAME test_gl)
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
    ${SDL2_INCLUDE_DIR}
)

# Link with some libraries
target_link_libraries(
    ${EXEC_NAME}
    PRIVATE
    OpenGL::GL
    ${SDL2_LIBRARY}
)
