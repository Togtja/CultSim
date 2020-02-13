target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/action.h

        ${CMAKE_CURRENT_LIST_DIR}/wait.h
        ${CMAKE_CURRENT_LIST_DIR}/wait.cpp

        ${CMAKE_CURRENT_LIST_DIR}/requirement.h

        ${CMAKE_CURRENT_LIST_DIR}/move_to.h
        ${CMAKE_CURRENT_LIST_DIR}/move_to.cpp
)
