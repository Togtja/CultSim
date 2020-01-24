cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(bachelor VERSION 0.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Project CMake Options
option(CULTSIM_BUILD_DOCS ON)

add_subdirectory(external)
add_subdirectory(bachelor)

# Build Documentation if enabled
if(CULTSIM_BUILD_DOCS)
    add_subdirectory(doc)
endif()
