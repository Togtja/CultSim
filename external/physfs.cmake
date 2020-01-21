# Set up sol2, either by finding the package directly, or by fetching it from GitHub
include(FetchContent)
message(STATUS "Could not find sol2 package so we are getting it from GitHub instead.")

# Declare where to find glfw and what version to use
FetchContent_Declare(
    physfs_external
    URL http://icculus.org/physfs/downloads/physfs-3.0.2.tar.bz2
)

# Populate it for building
FetchContent_MakeAvailable(physfs_external)
