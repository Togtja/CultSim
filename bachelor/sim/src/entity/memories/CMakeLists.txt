# Add source files for this directory

target_sources(
        ${EXEC_NAME}
        PRIVATE

        ${CMAKE_CURRENT_LIST_DIR}/memory.h
        
        ${CMAKE_CURRENT_LIST_DIR}/resource_memory.h

        ${CMAKE_CURRENT_LIST_DIR}/entity_memory.h

        ${CMAKE_CURRENT_LIST_DIR}/memory_container.h
)
