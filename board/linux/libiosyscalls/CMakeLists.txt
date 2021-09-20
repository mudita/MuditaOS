
project(iosyscalls VERSION 1.0
	DESCRIPTION "linux syscalls wrapper to our VFS"
)

set(INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/module-os/board/linux"
    "${CMAKE_SOURCE_DIR}/module-os/RTOSWrapper/include"
    "${CMAKE_SOURCE_DIR}/module-utils"
    "${CMAKE_SOURCE_DIR}/module-vfs/include/user"
    "${CMAKE_SOURCE_DIR}/module-vfs/include/internal"
)

add_library(${PROJECT_NAME} SHARED)

target_sources(
        ${PROJECT_NAME}

    PRIVATE
        src/interface.cpp
        src/iosyscalls-internal.hpp
        src/iosyscalls.cpp
        src/syscalls_posix_dirent.cpp
        src/syscalls_posix.cpp
        src/syscalls_real.hpp
        src/syscalls_scan_family.cpp
        src/syscalls_stdio.cpp

    PUBLIC
        include/debug.hpp
        include/iosyscalls.h
        include/iosyscalls.hpp
)

target_compile_options( ${PROJECT_NAME} PRIVATE "-Wno-nonnull-compare" )
get_target_property( target_options ${PROJECT_NAME} COMPILE_OPTIONS)
if("-fsanitize=address" IN_LIST target_options )
    list(REMOVE_ITEM target_options "-fsanitize=address")
    set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_OPTIONS ${target_options} )
endif()

get_target_property( target_options ${PROJECT_NAME} LINK_OPTIONS)
if("-fsanitize=address" IN_LIST target_options )
    list(REMOVE_ITEM target_options "-fsanitize=address")
    set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_OPTIONS ${target_options} )
endif()

target_link_options(${PROJECT_NAME} PRIVATE
		"-Wl,--version-script"
		"-Wl,${CMAKE_CURRENT_SOURCE_DIR}/version.txt"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES} )

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC
        dl
    PRIVATE
        phmap
        module-vfs
)

target_compile_definitions( ${PROJECT_NAME} PUBLIC TARGET_Linux )

