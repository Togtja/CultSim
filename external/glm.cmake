# Set up GLM by fetching it from GitHub
find_package(glm 0.9)

message(STATUS "Getting GLM from Github.")

# Declare where to find GLM and what version to use
FetchContent_Declare(
    glm_external
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.7
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(glm_external)
