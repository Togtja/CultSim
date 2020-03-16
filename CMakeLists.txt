cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(bachelor VERSION 0.2.0)

# Project CMake Options
option(CULTSIM_BUILD_DOCS "Build documentation for Cultsim" OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/external ${CMAKE_MODULE_PATH})

#Add extra libs for VisualStudio compilers
if(MSVC)
    list(APPEND EXTRA_LIBS vcruntime user32 gdi32 winmm imm32 ole32 oleaut32 version uuid advapi32 setupapi shell32)
endif()

# Set CULTSIM_DEBUG so we can know if we're in debug mode or not
if(CMAKE_BUILD_TYPE STREQUAL "[Dd]ebug")
    set(CULTSIM_DEBUG 1)
else()
    set(CULTSIM_DEBUG 0)
endif()

# Enable CTest
include(CTest)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/doctest.cmake)

# Add dependencies and our executables
add_subdirectory(external)
add_subdirectory(bachelor)

# Build Documentation if enabled
if(CULTSIM_BUILD_DOCS)
    add_subdirectory(docs)
endif()
