# The main CMakeLists.txt file for building the entire bachelor's project and all supporting libraries
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(bachelor_project)

# Disable symbol visibility in libraries by default
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Enable using CTest if we are the root project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

# Include doctest CTest integration utility function
include(${CMAKE_CURRENT_LIST_DIR}/cmake/doctest.cmake)

# Find dependencies in subfolder and include their scripts
file(GLOB BP_DEPENDENCIES LIST_DIRECTORIES false ${CMAKE_CURRENT_LIST_DIR}/external/*.cmake)
foreach(DEP IN LISTS BP_DEPENDENCIES)
    include(${DEP})
endforeach(DEP IN LISTS BP_DEPENDENCIES)

# Add rendering library
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/renderer)
