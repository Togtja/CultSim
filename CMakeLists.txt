cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(bachelor VERSION 0.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Project CMake Options
option(CULTSIM_BUILD_DOCS ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/external ${CMAKE_MODULE_PATH})

add_subdirectory(external)
add_subdirectory(bachelor)

# Build Documentation if enabled
if(CULTSIM_BUILD_DOCS)
    add_subdirectory(docs)
endif()
