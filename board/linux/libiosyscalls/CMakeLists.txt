
project(iosyscalls VERSION 1.0
	DESCRIPTION "linux syscalls wrapper to our VFS"
)


set(SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/src/syscalls_stdio.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/syscalls_scan_family.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/syscalls_posix.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/dirent_support.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/dirent.cpp

)

set(INCLUDES
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/module-vfs/board/freeRTOS_FAT/include"
	"${CMAKE_SOURCE_DIR}/module-os/FreeRTOS/include"
	"${CMAKE_SOURCE_DIR}/module-os/board/linux"
	"${CMAKE_SOURCE_DIR}/module-vfs/board/free_rtos_custom/include"
	"${CMAKE_SOURCE_DIR}/module-utils"
)


add_library(${PROJECT_NAME} SHARED ${SOURCES})

get_target_property( target_options ${PROJECT_NAME} COMPILE_OPTIONS)
list(REMOVE_ITEM target_options "-fsanitize=address")
set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_OPTIONS ${target_options} )

get_target_property( target_options ${PROJECT_NAME} LINK_OPTIONS)
list(REMOVE_ITEM target_options "-fsanitize=address")
set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_OPTIONS ${target_options} )

target_link_options(${PROJECT_NAME} PRIVATE
		"-Wl,--version-script"
		"-Wl,${CMAKE_CURRENT_SOURCE_DIR}/version.txt"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES} )
target_link_libraries( ${PROJECT_NAME} dl )
target_compile_definitions( ${PROJECT_NAME} PUBLIC TARGET_Linux )
