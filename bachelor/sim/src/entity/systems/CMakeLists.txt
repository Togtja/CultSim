target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/action_test.cpp
        ${CMAKE_CURRENT_LIST_DIR}/action.cpp
        ${CMAKE_CURRENT_LIST_DIR}/action.h

        ${CMAKE_CURRENT_LIST_DIR}/health.cpp
        ${CMAKE_CURRENT_LIST_DIR}/health.h

        ${CMAKE_CURRENT_LIST_DIR}/inventory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/inventory.h

        ${CMAKE_CURRENT_LIST_DIR}/memory.cpp
        ${CMAKE_CURRENT_LIST_DIR}/memory.h

        ${CMAKE_CURRENT_LIST_DIR}/mitigation_test.cpp
        ${CMAKE_CURRENT_LIST_DIR}/mitigation.cpp
        ${CMAKE_CURRENT_LIST_DIR}/mitigation.h

        ${CMAKE_CURRENT_LIST_DIR}/movement.cpp
        ${CMAKE_CURRENT_LIST_DIR}/movement.h

        ${CMAKE_CURRENT_LIST_DIR}/need_test.cpp
        ${CMAKE_CURRENT_LIST_DIR}/need.cpp
        ${CMAKE_CURRENT_LIST_DIR}/need.h

        ${CMAKE_CURRENT_LIST_DIR}/rendering.cpp
        ${CMAKE_CURRENT_LIST_DIR}/rendering.h

        ${CMAKE_CURRENT_LIST_DIR}/reproduction.cpp
        ${CMAKE_CURRENT_LIST_DIR}/reproduction.h

        ${CMAKE_CURRENT_LIST_DIR}/requirement.cpp
        ${CMAKE_CURRENT_LIST_DIR}/requirement.h

        ${CMAKE_CURRENT_LIST_DIR}/sensor.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sensor.h

        ${CMAKE_CURRENT_LIST_DIR}/system.h

        ${CMAKE_CURRENT_LIST_DIR}/timer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/timer.h
              
        
        ${CMAKE_CURRENT_LIST_DIR}/goal.cpp
        ${CMAKE_CURRENT_LIST_DIR}/goal.h
        
        ${CMAKE_CURRENT_LIST_DIR}/gob.cpp
        ${CMAKE_CURRENT_LIST_DIR}/gob.h

        ${CMAKE_CURRENT_LIST_DIR}/deletion.cpp
        ${CMAKE_CURRENT_LIST_DIR}/deletion.h
)
