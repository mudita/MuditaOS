include(ExternalProject)

ExternalProject_Add(CrashDebug
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/src CrashDebug
    INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/bin && ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/src/bins/lin64/CrashDebug ${CMAKE_CURRENT_BINARY_DIR}/bin
)

add_subdirectory(CrashCatcher)
