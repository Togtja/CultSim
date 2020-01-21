# Set up sdl2, either by finding the package directly, or by fetching it from GitHub

include(FetchContent)  
message(STATUS "Could not find sdl2 package so we are downloading it.")
# Declare where to find SDL2 and what version to use
FetchContent_Declare(
    sdl2_external
    URL https://www.libsdl.org/release/SDL2-2.0.10.tar.gz
)

# Populate it for building
FetchContent_MakeAvailable(sdl2_external)
