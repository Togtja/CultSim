# The ___ Renderer CMakeLists

# Assign a name to the renderer library
set(BP_RENDERER_NAME ulfeye)

# Create the library
add_library(${BP_RENDERER_NAME} ${CMAKE_BUILD_SHARED_LIBS})

# Include CMakeLists files in subdirectories
# These files specify sources or other operations they need
include(${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/res/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/include/CMakeLists.txt)

