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
        ${CMAKE_CURRENT_LIST_DIR}/need_test.cpp

        ${CMAKE_CURRENT_LIST_DIR}/mitigation.h
        ${CMAKE_CURRENT_LIST_DIR}/mitigation.cpp
        ${CMAKE_CURRENT_LIST_DIR}/mitigation_test.cpp

        ${CMAKE_CURRENT_LIST_DIR}/action.h
        ${CMAKE_CURRENT_LIST_DIR}/action.cpp
        ${CMAKE_CURRENT_LIST_DIR}/action_test.cpp

        ${CMAKE_CURRENT_LIST_DIR}/requirement.h
        ${CMAKE_CURRENT_LIST_DIR}/requirement.cpp

        ${CMAKE_CURRENT_LIST_DIR}/timer.h
        ${CMAKE_CURRENT_LIST_DIR}/timer.cpp

        ${CMAKE_CURRENT_LIST_DIR}/health.h
        ${CMAKE_CURRENT_LIST_DIR}/health.cpp


        ${CMAKE_CURRENT_LIST_DIR}/reproduction.h
        ${CMAKE_CURRENT_LIST_DIR}/reproduction.cpp

)
