target_sources(
        ${EXEC_NAME}
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/debug_macros.h

        ${CMAKE_CURRENT_LIST_DIR}/auto_timer.h
        ${CMAKE_CURRENT_LIST_DIR}/auto_timer.cpp

        ${CMAKE_CURRENT_LIST_DIR}/locale_editor.h
        ${CMAKE_CURRENT_LIST_DIR}/locale_editor.cpp

        ${CMAKE_CURRENT_LIST_DIR}/data_collector.h
        ${CMAKE_CURRENT_LIST_DIR}/data_collector.cpp
)
