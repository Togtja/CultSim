# Tests for the renderer
target_sources(
    ${BP_RENDERER_TEST_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_link_libraries(
    ${BP_RENDERER_TEST_NAME}
    PRIVATE
    ${BP_RENDERER_NAME}
    doctest::doctest
)
