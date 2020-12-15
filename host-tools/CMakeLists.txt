if (CMAKE_CROSSCOMPILING)
    # Littlefs fuse is needed in rt1051 and Linux for manipulate images
    # genlittlefs is needed only on the Linux image for generate emulator target image
    add_custom_target(
        lfsfuse ALL
        COMMAND ${CMAKE_COMMAND}
        -DCMAKE_BUILD_TYPE:STRING="Release"
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH="${CMAKE_BINARY_DIR}"
        -B"build"
        -H"${CMAKE_SOURCE_DIR}/host-tools/littlefs-fuse"
        COMMAND ${CMAKE_COMMAND} --build build --config Release
    )
else()
    set(_genlittlefs "${CMAKE_BINARY_DIR}/genlittlefs${CMAKE_EXECUTABLE_SUFFIX}")
    add_subdirectory(genlittlefs)
    add_subdirectory(littlefs-fuse)
endif()

