# Set up sol2 by fetching it from GitHub
message(STATUS "Could not find sol2 package so we are getting it from GitHub instead.")

# Declare where to find glfw and what version to use
FetchContent_Declare(
    sol2_external
    GIT_REPOSITORY https://github.com/ThePhD/sol2.git
    GIT_TAG v3.2.0
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(sol2_external)
