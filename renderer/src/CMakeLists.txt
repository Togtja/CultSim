# Add source files
target_sources(
        ${BP_RENDERER_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/ulfeyes.cpp
        ${CMAKE_CURRENT_LIST_DIR}/test_export.cpp

        # Context contain abstraction for Instance / Device / PhysicalDevice and utilities related to managing / creating them
        ${CMAKE_CURRENT_LIST_DIR}/context.hpp
        ${CMAKE_CURRENT_LIST_DIR}/context.cpp

        # Helpers.h/cpp contains helper functions related to Vulkan
        ${CMAKE_CURRENT_LIST_DIR}/helpers.hpp
        ${CMAKE_CURRENT_LIST_DIR}/helpers.cpp

        # Swapchain contain abstraction for Swapchain and utilities related to managing / creating one
        ${CMAKE_CURRENT_LIST_DIR}/swapchain.hpp
        ${CMAKE_CURRENT_LIST_DIR}/swapchain.cpp
)

target_include_directories(
        ${BP_RENDERER_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
