cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(bachelor VERSION 0.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

add_subdirectory(external)
add_subdirectory(bachelor)
