# Set up GLM, either by finding the package directly, or by fetching it from GitHub
include(FetchContent)
message(STATUS "Could not find GLM package so we are getting it from GitHub instead.")

# Declare where to find GLM and what version to use
FetchContent_Declare(
    glm_external
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.7
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(glm_external)
