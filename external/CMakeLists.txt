# Set some FETCH CONTENT variables to ensure we don't over-download content

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# Include All Dependencies
include(benchmark.cmake)
include(sol2.cmake)
add_subdirectory(glad)
add_subdirectory(pcg)
add_subdirectory(taskflow)
include(physfs.cmake)
include(sdl2.cmake)
include(entt.cmake)
include(spdlog.cmake)
include(doctest.cmake)
include(glm.cmake)


add_subdirectory(lunatic-python)

# Set FC to be disconnected so we stop downloading
set(FETCHCONTENT_FULLY_DISCONNECTED ON CACHE BOOL "This has been reset to ON (set to OFF to download again)" FORCE)
