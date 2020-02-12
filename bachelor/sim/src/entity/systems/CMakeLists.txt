target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/system.h

        ${CMAKE_CURRENT_LIST_DIR}/ai.h
        ${CMAKE_CURRENT_LIST_DIR}/ai.cpp

        ${CMAKE_CURRENT_LIST_DIR}/movement.h
        ${CMAKE_CURRENT_LIST_DIR}/movement.cpp

        ${CMAKE_CURRENT_LIST_DIR}/rendering.h
        ${CMAKE_CURRENT_LIST_DIR}/rendering.cpp

        ${CMAKE_CURRENT_LIST_DIR}/need.h
        ${CMAKE_CURRENT_LIST_DIR}/need.cpp

        ${CMAKE_CURRENT_LIST_DIR}/mitigation.h
        ${CMAKE_CURRENT_LIST_DIR}/mitigation.cpp

        ${CMAKE_CURRENT_LIST_DIR}/action.h
        ${CMAKE_CURRENT_LIST_DIR}/action.cpp
)
