# Set up sol2, either by finding the package directly, or by fetching it from GitHub
include(FetchContent)
message(STATUS "Could not find PhysFS package so we are downloading it.")

# Declare where to find glfw and what version to use
FetchContent_Declare(
    physfs_external
    URL http://icculus.org/physfs/downloads/physfs-3.0.2.tar.bz2
    URL_HASH SHA256=304df76206d633df5360e738b138c94e82ccf086e50ba84f456d3f8432f9f863
)

# Populate it for building
FetchContent_MakeAvailable(physfs_external)

if(WIN32)
    set(PHYSFS_LIBRARY ${PROJECT_BINARY_DIR}/_deps/physfs_external-build/physfs.lib)
    set(PHYSFS_INCLUDE_DIR ${PROJECT_BINARY_DIR}/_deps/physfs_external-src/src)
endif()
