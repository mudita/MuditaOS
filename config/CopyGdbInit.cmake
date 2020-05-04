if ($ENV{JETBRAINS_IDE})
    set(CLION_IDE True CACHE BOOL "Build under CLion")
endif ()

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set(GDB_INIT_SCRIPT "${CMAKE_SOURCE_DIR}/.gdbinit")
else()
    if(CLION_IDE)
        set(GDB_INIT_SCRIPT "${CMAKE_SOURCE_DIR}/.gdbinit-1051-clion")
    else()
        set(GDB_INIT_SCRIPT "${CMAKE_SOURCE_DIR}/.gdbinit-1051")
    else()
    endif()
endif()

configure_file(${GDB_INIT_SCRIPT} ${CMAKE_BINARY_DIR}/.gdbinit COPYONLY)

