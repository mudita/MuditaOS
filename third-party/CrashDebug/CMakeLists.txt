include(ExternalProject)

ExternalProject_Add(CrashDebug
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/src CrashDebug
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/bin && ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/bins/lin64/CrashDebug ${CMAKE_CURRENT_BINARY_DIR}/bin
)

add_library(CrashCatcher INTERFACE)
add_library(CrashCatcher::CrashCatcher ALIAS CrashCatcher)

target_include_directories(CrashCatcher
    INTERFACE
        src/CrashCatcher/include
        src/CrashCatcher/Core/src
)

target_sources(CrashCatcher
    INTERFACE
        src/CrashCatcher/Core/src/CrashCatcher.c
        CrashCatcher_armv7m.S
)

set_source_files_properties(CrashCatcher_armv7m.S PROPERTIES LANGUAGE C)
