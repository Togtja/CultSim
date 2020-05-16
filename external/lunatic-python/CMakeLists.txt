cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_BUILD_TYPE_INIT "Release")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})


project(Lunatic)

set(LUA_INCLUDE_DIR "D:/_Programming/Path_bin/lua/lua53/include/" CACHE STRING "The path of include of lua")
set(LUA_LIBRARIES "D:/_Programming/Path_bin/lua/lua53/lua5.3.lib" CACHE STRING "The path of lua53.lib")

set(PYTHON_LIBRARIES "C:/Users/Tomas/AppData/Local/Programs/Python/Python38/libs" CACHE STRING "The path of python38.lib")

find_package(Lua        REQUIRED)
find_package(PythonLibs REQUIRED)


add_subdirectory(src)

add_library(python MODULE $<TARGET_OBJECTS:src>)
set_target_properties(python PROPERTIES 
                             PREFIX "")

add_library(lua MODULE $<TARGET_OBJECTS:src>)
if (WIN32)
  set_target_properties(lua PROPERTIES 
                            PREFIX ""
                            SUFFIX ".pyd")
else ()
  set_target_properties(lua PROPERTIES 
                            PREFIX "")
endif ()

target_link_libraries(lua     ${LUA_LIBRARIES} ${PYTHON_LIBRARIES})
target_link_libraries(python  ${LUA_LIBRARIES} ${PYTHON_LIBRARIES})
