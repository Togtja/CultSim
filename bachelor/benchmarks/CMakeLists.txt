# Create Benchmarking executable

# Define executable
set(EXEC_NAME cultbench)
add_executable(${EXEC_NAME})

# Add Source Files
target_sources(
        cultbench
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/functions.h
        ${CMAKE_CURRENT_LIST_DIR}/functions.cpp
)

# Set Include Directories
target_include_directories(
        ${EXEC_NAME}
        PRIVATE
)

# Link with some libraries
# find_package(OpenMP REQUIRED)

target_link_libraries(
        ${EXEC_NAME}
        PRIVATE
        benchmark
        EnTT::EnTT
        glm::glm
        # OpenMP::OpenMP_CXX
)

# Explicitly enable C++ 17 for this executable
target_compile_features(
        ${EXEC_NAME}
        PRIVATE
        cxx_std_17
)
