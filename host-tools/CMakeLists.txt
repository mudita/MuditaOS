include(ExternalProject)

if (CMAKE_CROSSCOMPILING)
    # Littlefs fuse is needed in rt1051 and Linux for manipulate images
    # genlittlefs is needed only on the Linux image for generate emulator target image
    add_custom_target(
        lfsfuse ALL
        COMMAND ${CMAKE_COMMAND}
        -DCMAKE_BUILD_TYPE:STRING="Release"
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH="${CMAKE_BINARY_DIR}"
        -B"lfsfuse"
        -H"${CMAKE_SOURCE_DIR}/host-tools/littlefs-fuse"
        COMMAND ${CMAKE_COMMAND} --build lfsfuse --config Release
    )
    add_custom_target(
        genlittlefs ALL
        COMMAND ${CMAKE_COMMAND}
        -DCMAKE_BUILD_TYPE:STRING="Release"
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH="${CMAKE_BINARY_DIR}"
        -B"genlittlefs"
        -H"${CMAKE_SOURCE_DIR}/host-tools/genlittlefs"
        COMMAND ${CMAKE_COMMAND} --build genlittlefs --config Release
    )
    ExternalProject_Add(pureflash
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/pure-flash
        INSTALL_DIR ${PROJECT_BINARY_DIR}
        CMAKE_ARGS
            -D CMAKE_BUILD_TYPE=Release
            -D CMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    )
else()
    set(_genlittlefs "${CMAKE_BINARY_DIR}/genlittlefs${CMAKE_EXECUTABLE_SUFFIX}")
    add_subdirectory(genlittlefs)
    add_subdirectory(littlefs-fuse)
endif()
