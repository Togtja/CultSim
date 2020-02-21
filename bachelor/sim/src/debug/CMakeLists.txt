target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/debug_macros.h

        ${CMAKE_CURRENT_LIST_DIR}/auto_timer.h
        ${CMAKE_CURRENT_LIST_DIR}/auto_timer.cpp
)
