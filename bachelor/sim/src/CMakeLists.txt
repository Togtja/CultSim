# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp

        ${CMAKE_CURRENT_LIST_DIR}/robin_hood.h

        ${CMAKE_CURRENT_LIST_DIR}/application.h
        ${CMAKE_CURRENT_LIST_DIR}/application.cpp

        ${CMAKE_CURRENT_LIST_DIR}/delta_clock.h
        ${CMAKE_CURRENT_LIST_DIR}/delta_clock.cpp

        ${CMAKE_CURRENT_LIST_DIR}/preferences.h
        ${CMAKE_CURRENT_LIST_DIR}/preferences.cpp

        ${CMAKE_CURRENT_LIST_DIR}/random_engine.h
        ${CMAKE_CURRENT_LIST_DIR}/random_engine.cpp

        ${CMAKE_CURRENT_LIST_DIR}/lua_type_bindings.h
        ${CMAKE_CURRENT_LIST_DIR}/lua_type_bindings.cpp

        ${CMAKE_CURRENT_LIST_DIR}/common_helpers.h
        ${CMAKE_CURRENT_LIST_DIR}/common_helpers.cpp
)

# Configure file with CMake Variables
configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/constants.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/constants.h
)
