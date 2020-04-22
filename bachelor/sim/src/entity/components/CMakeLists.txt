# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/components.h

        ${CMAKE_CURRENT_LIST_DIR}/need.h

        ${CMAKE_CURRENT_LIST_DIR}/strategy.h

        ${CMAKE_CURRENT_LIST_DIR}/tags.h

        ${CMAKE_CURRENT_LIST_DIR}/action.h

        ${CMAKE_CURRENT_LIST_DIR}/goal.h
)
