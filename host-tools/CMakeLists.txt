if (CMAKE_CROSSCOMPILING)
    # Littlefs fuse is needed in rt1051 and Linux for manipulate images
    # genlittlefs is needed only on the Linux image for generate emulator target image
    set(CRASH_DEBUG_DIR "${CMAKE_SOURCE_DIR}/module-utils/CrashDebug")
    add_custom_target(
        crashdebug ALL
        COMMAND make -C ${CRASH_DEBUG_DIR} CrashDebug
        COMMAND ${CMAKE_COMMAND} -E make_directory crashdebug
        COMMAND ${CMAKE_COMMAND} -E copy ${CRASH_DEBUG_DIR}/bins/lin64/CrashDebug crashdebug
    )
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
    add_custom_target(
        pureflash
        COMMAND ${CMAKE_COMMAND}
        -DCMAKE_BUILD_TYPE:STRING="Release"
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:PATH="${CMAKE_BINARY_DIR}"
        -B"pureflash"
        -H"${CMAKE_SOURCE_DIR}/host-tools/pure-flash"
        COMMAND ${CMAKE_COMMAND} --build pureflash --config Release
    )

else()
    set(_genlittlefs "${CMAKE_BINARY_DIR}/genlittlefs${CMAKE_EXECUTABLE_SUFFIX}")
    add_subdirectory(genlittlefs)
    add_subdirectory(littlefs-fuse)
endif()

