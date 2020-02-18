# Add vma as a Library that we can link with

add_library(vma ${BUILD_SHARED_LIBS})
add_library(vma::vma ALIAS vma)

# Add source files
target_sources(
        vma

        # Public Sources
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/vma.h

        # Private Sources
        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/vma.cpp
)

# Set include directories
target_include_directories(
        vma
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)
