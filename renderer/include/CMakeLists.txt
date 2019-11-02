# Add include files (public)
target_sources(
        ${BP_RENDERER_NAME}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/ulfeyes.hpp
)

target_include_directories(
        ${BP_RENDERER_NAME}
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
