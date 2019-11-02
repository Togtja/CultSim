# Set up doctest, by fetching it from GitHub
include(FetchContent)
message(STATUS "Fetching doctest from GitHub using FetchContent.")

# Declare where to find doctest and what version to use
FetchContent_Declare(
    doctest_external
    GIT_REPOSITORY https://github.com/onqtam/doctest
    GIT_TAG 2.3.5
    GIT_PROGRESS TRUE
)

# Populate and make available
FetchContent_MakeAvailable(doctest_external)
