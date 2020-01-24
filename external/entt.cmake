# Set up EnTT, either by finding the package directly, or by fetching it from GitHub
include(FetchContent)
message(STATUS "Could not find EnTT package so we are getting it from GitHub instead.")

# Declare where to find EnTT and what version to use
FetchContent_Declare(
    entt_external
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.2.2
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(entt_external)