# Set up DocTest, either by finding the package directly, or by fetching it from GitHub
message(STATUS "Could not find DocTest package so we are getting it from GitHub instead.")

# Declare where to find DocTest and what version to use
FetchContent_Declare(
    doctest_external
    GIT_REPOSITORY https://github.com/onqtam/doctest.git
    GIT_TAG 2.3.6
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(doctest_external)
