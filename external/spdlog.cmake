# Set up spdlog, either by finding the package directly, or by fetching it from GitHub
message(STATUS "Could not find spdlog package so we are getting it from GitHub instead.")

# Declare where to find spdlog and what version to use
FetchContent_Declare(
    spdlog_external
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.5.0
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(spdlog_external)
