
project(iosyscalls VERSION 1.0
	DESCRIPTION "linux syscalls wrapper to our VFS"
)


set(SOURCES
	 ${CMAKE_CURRENT_SOURCE_DIR}/src/syscalls.c
)


add_library(${PROJECT_NAME} SHARED ${SOURCES})

get_target_property( target_options ${PROJECT_NAME} COMPILE_OPTIONS)
list(REMOVE_ITEM target_options "-fsanitize=address")
set_property(TARGET ${PROJECT_NAME} PROPERTY COMPILE_OPTIONS ${target_options} )

get_target_property( target_options ${PROJECT_NAME} LINK_OPTIONS)
list(REMOVE_ITEM target_options "-fsanitize=address")
set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_OPTIONS ${target_options} )

target_link_options(${PROJECT_NAME} PRIVATE "-Wl,--version-script" "-Wl,${CMAKE_CURRENT_SOURCE_DIR}/version.txt")

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries( ${PROJECT_NAME} dl )
