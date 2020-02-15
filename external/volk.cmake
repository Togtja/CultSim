# Set up volk by fetching it from GitHub
message(STATUS "Getting GLM from Github.")

# Declare where to find volk and what version to use
FetchContent_Declare(
        volk_external
        GIT_REPOSITORY https://github.com/zeux/volk.git
        GIT_TAG 2638ad1b2b40f1ad402a0a6ac55b60bc51a23058
)

# Populate it for building
FetchContent_MakeAvailable(volk_external)
