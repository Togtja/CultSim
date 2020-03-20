# pcg Random Generator CMakeLists

add_library(pcg INTERFACE)
add_library(pcg::pcg ALIAS pcg)

# Add files as usage requirements
target_sources(
    pcg
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pcg/pcg_extras.hpp
    ${CMAKE_CURRENT_LIST_DIR}/pcg/pcg_random.hpp
    ${CMAKE_CURRENT_LIST_DIR}/pcg/pcg_uint128.hpp
)

# Make sure we can include pcg/xxx
target_include_directories(
    pcg
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
