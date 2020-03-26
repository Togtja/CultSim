# Add Taskflow as a Library that we can link with

add_library(taskflow INTERFACE)
add_library(taskflow::taskflow ALIAS taskflow)

# Add source files
target_sources(
    taskflow

    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/taskflow.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/declarations.hpp

    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/error.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/executor.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/flow_builder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/graph.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/notifier.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/observer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/task.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/taskflow.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/topology.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/core/tsq.hpp

    ${CMAKE_CURRENT_LIST_DIR}/taskflow/cuda/device.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/cuda/error.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/cuda/flow_builder.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/cuda/graph.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/cuda/task.hpp

    ${CMAKE_CURRENT_LIST_DIR}/taskflow/nstd/any.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/nstd/variant.hpp

    ${CMAKE_CURRENT_LIST_DIR}/taskflow/utility/object_pool.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/utility/passive_vector.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/utility/stringify.hpp
    ${CMAKE_CURRENT_LIST_DIR}/taskflow/utility/traits.hpp
)

# Set include directories
target_include_directories(
    taskflow
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
