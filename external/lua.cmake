# Set up lua
if(WIN32)
    include(FetchContent)
    message(STATUS "Could not find Lua package so getting it")

    if(MSVC14)
        FetchContent_Declare(
            lua_external
            URL https://netix.dl.sourceforge.net/project/luabinaries/5.3.5/Windows%20Libraries/Static/lua-5.3.5_Win64_vc15_lib.zip
            URL_HASH SHA256=a477694d5f38116dc71c8b1ecb25e8dbb3361dc457cc9502b56aa7ce1c0dfe12
        )
        FetchContent_MakeAvailable(lua_external)
    elseif(MSVC12)
        FetchContent_Declare(
            lua_external
            URL https://iweb.dl.sourceforge.net/project/luabinaries/5.3.5/Windows%20Libraries/Static/lua-5.3.5_Win64_vc12_lib.zip
            URL_HASH SHA256=7d41206041cd34483f74a689007331d728aa39520c65c87d4141dbb6d8ba043f
        )
        FetchContent_MakeAvailable(lua_external)
    elseif(MINGW)
        FetchContent_Declare(
            lua_external
            URL https://netcologne.dl.sourceforge.net/project/luabinaries/5.3.5/Windows%20Libraries/Static/lua-5.3.5_Win64_mingw6_lib.zip
            URL_HASH SHA256=a477694d5f38116dc71c8b1ecb25e8dbb3361dc457cc9502b56aa7ce1c0dfe12
        )
        FetchContent_MakeAvailable(lua_external)
    else()
        message(FATAL_ERROR "Does not support this compier for auto downloading ua, you have to manually download Lua")
    endif()

    #Bad cmake
    set(LUA_INCLUDE_DIR "${CMAKE_BINARY_DIR}/_deps/lua_external-src/include/" CACHE STRING "The path of include of lua")
    set(LUA_LIBRARIES "${CMAKE_BINARY_DIR}/_deps/lua_external-src/lua53.lib" CACHE STRING "The path of lua53.lib")

endif()
