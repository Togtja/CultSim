# This CMake file makes Doxygen generate documentation
# Passes it through Breathe and Sphinx too
# Source for making it work: https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/

find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_GENERATE_XML YES)
set(DOXYGEN_GENERATE_HTML NO)

doxygen_add_docs(
    cultsim_docs
    ${CMAKE_SOURCE_DIR}/bachelor/sim
    ALL
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating initial Doxygen documentation..."
)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)

add_custom_target(
    cultsim_sphinx ALL
    COMMAND ${SPHINX_EXECUTABLE} -b html
    -Dbreathe_projects.Cultsim=${CMAKE_CURRENT_BINARY_DIR}/xml
    ${SPHINX_SOURCE} ${SPHINX_BUILD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating documentation with Sphinx..."
)

add_dependencies(cultsim_sphinx cultsim_docs)
