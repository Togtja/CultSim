# Add Ultralight as a Library that we can link with

add_library(ultralight INTERFACE)
add_library(ultralight::ultralight ALIAS ultralight)

# Add source files
target_sources(
    ultralight

    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/platform/Config.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/platform/FileSystem.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/platform/FontLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/platform/GPUDriver.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/platform/Platform.h

    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/Bitmap.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/Buffer.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/CAPI.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/Defines.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/Geometry.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/KeyCodes.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/KeyEvent.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/Listener.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/Matrix.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/MouseEvent.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/RefPtr.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/Renderer.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/RenderTarget.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/ScrollEvent.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/String.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/String8.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/String16.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/String32.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/Ultralight.h
    ${CMAKE_CURRENT_LIST_DIR}/Ultralight/View.h
)

# Set include directories
target_include_directories(
    ultralight
    INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)
