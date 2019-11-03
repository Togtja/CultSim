# The ___ Renderer CMakeLists

# Assign a name to the renderer library
set(BP_RENDERER_NAME ulfeye)
set(BP_RENDERER_TEST_NAME ${BP_RENDERER_NAME}_tests)

# Create the library (we use SHARED here to make tests export correctly to test executable)
# Normally I would use BUILD_SHARED_LIBS so the user can decide, but for tests to work this is currently hard coded to SHARED
add_library(${BP_RENDERER_NAME} SHARED)
target_compile_features(${BP_RENDERER_NAME} PRIVATE cxx_std_17)

# Include CMakeLists files in subdirectories
# These files specify sources and/or include directories or other required resources
include(${CMAKE_CURRENT_LIST_DIR}/src/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/res/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/include/CMakeLists.txt)

# Create testing executable
add_executable(${BP_RENDERER_TEST_NAME})
target_compile_features(${BP_RENDERER_TEST_NAME} PRIVATE cxx_std_17)
include(${CMAKE_CURRENT_LIST_DIR}/test/CMakeLists.txt)

# Find libraries that are non-buildable, downloadable or otherwise should be accessible as a package
find_package(Vulkan REQUIRED)

# Link libraries required by this library
target_link_libraries(
    ${BP_RENDERER_NAME}
    PUBLIC
    Vulkan::Vulkan

    PRIVATE
    doctest::doctest
)
