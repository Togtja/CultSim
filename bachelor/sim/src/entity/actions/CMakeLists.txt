target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/action.h

        ${CMAKE_CURRENT_LIST_DIR}/requirement.h

        ${CMAKE_CURRENT_LIST_DIR}/timed_action.h
        ${CMAKE_CURRENT_LIST_DIR}/timed_action.cpp

)
