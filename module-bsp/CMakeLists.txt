cmake_minimum_required(VERSION 3.12)

project(module-bsp VERSION 1.0
        DESCRIPTION "VFS module library")

if(NOT DEFINED PROJECT_LIB_DIRECTORY )
    set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
    message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB_DIRECTORY})

if(NOT DEFINED PROJECT_BUILD_DIRECTORY )
    set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building" )
    message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")
endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIRECTORY})

if(NOT ${PROJECT_TARGET}  STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
endif()

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES} )


# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

target_compile_definitions(${PROJECT_NAME}

        PUBLIC
        -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1
        -D__STARTUP_INITIALIZE_NONCACHEDATA
        -D__USE_CMSIS
)

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/bsp
        ${CMAKE_CURRENT_LIST_DIR}/bsp/keyboard


)
target_include_directories( ${PROJECT_NAME}  
	PUBLIC 
		"${CMAKE_SOURCE_DIR}/"
		"${CMAKE_SOURCE_DIR}/bsp"
		"${CMAKE_SOURCE_DIR}/bsp/keyboard"
		"${CMAKE_SOURCE_DIR}/bsp/rtc"
		"${CMAKE_SOURCE_DIR}/module-services/"  
)

set_source_files_properties(${BOARD_SOURCE_FILES_PROPERTIES})

target_link_libraries(${PROJECT_NAME} module-os module-services ${TARGET_LIBRARIES})

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)
    add_subdirectory(tests)
endif()

