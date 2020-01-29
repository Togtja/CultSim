# Set up sdl2, either by finding the package directly, or by fetching it from GitHub
message(STATUS "Downloading SDL2 dependency...")

# Declare where to find SDL2 and what version to use
FetchContent_Declare(
    sdl2_external
    URL https://www.libsdl.org/release/SDL2-2.0.10.tar.gz
    URL_HASH SHA256=b4656c13a1f0d0023ae2f4a9cf08ec92fffb464e0f24238337784159b8b91d57
)

# Populate it for building
FetchContent_MakeAvailable(sdl2_external)
