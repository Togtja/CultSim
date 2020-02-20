target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/auto_timer.h
        ${CMAKE_CURRENT_LIST_DIR}/auto_timer.cpp

        ${CMAKE_CURRENT_LIST_DIR}/debug_timer_manager.h
        ${CMAKE_CURRENT_LIST_DIR}/debug_timer_manager.cpp
)
