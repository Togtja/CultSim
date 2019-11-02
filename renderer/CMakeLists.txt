# The ___ Renderer CMakeLists

# Assign a name to the renderer library
set(BP_RENDERER_NAME ulfeye)
set(BP_RENDERER_TEST_NAME ${BP_RENDERER_NAME}_tests)

# Create the library
add_library(${BP_RENDERER_NAME} ${CMAKE_BUILD_SHARED_LIBS})

# Create testing executable
add_executable(${BP_RENDERER_TEST_NAME})

# Include CMakeLists files in subdirectories
# These files specify sources and/or include directories or other required resources
include(${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/res/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/include/CMakeLists.txt)

# Include subdirectories for the tests
include(${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)
