# Set some FETCH CONTENT variables to ensure we don't over-download content

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR})

# Include All Dependencies
include(benchmark.cmake)
include(sol2.cmake)
add_subdirectory(glad)
include(physfs.cmake)
include(sdl2.cmake)
include(lua.cmake)
include(entt.cmake)
include(spdlog.cmake)
include(doctest.cmake)
include(glm.cmake)
