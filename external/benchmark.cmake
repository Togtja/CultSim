# Set up GoogleBenchmark, either by finding the package directly, or by fetching it from GitHub
message(STATUS "Getting Google Benchmark from GitHub.")

# Disable testing library
option(BENCHMARK_ENABLE_GTEST_TESTS OFF)
option(BENCHMARK_ENABLE_ASSEMBLY_TESTS OFF)
option(BENCHMARK_ENABLE_TESTING OFF)

# Declare where to find glfw and what version to use
FetchContent_Declare(
    benchmark_external
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.5.0
    GIT_PROGRESS TRUE
)

# Populate it for building
FetchContent_MakeAvailable(benchmark_external)
