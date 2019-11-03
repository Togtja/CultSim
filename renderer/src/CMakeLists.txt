# Add source files
target_sources(
        ${BP_RENDERER_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/ulfeyes.cpp
        ${CMAKE_CURRENT_LIST_DIR}/test_export.cpp
)

target_include_directories(
        ${BP_RENDERER_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
