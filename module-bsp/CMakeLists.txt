cmake_minimum_required(VERSION 3.12)

project(module-bsp VERSION 1.0
        DESCRIPTION "VFS module library")

if (NOT DEFINED PROJECT_LIB_DIRECTORY)
    set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
    message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
endif ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB_DIRECTORY})

if (NOT DEFINED PROJECT_BUILD_DIRECTORY)
    set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building")
    message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIRECTORY})

set(SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/bsp/cellular/bsp_cellular.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/bsp/audio/bsp_audio.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/drivers/DriverInterface.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/drivers/pll/DriverPLL.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/drivers/dmamux/DriverDMAMux.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/drivers/dma/DriverDMA.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/drivers/i2c/DriverI2C.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/drivers/sai/DriverSAI.cpp"
        )

if (NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
endif ()

if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else ()
    message(FATAL_ERROR "Invalid target!")
endif ()

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})


# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES}) #M.P: fix me, should be declared as PRIVATE
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
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
        "${CMAKE_CURRENT_SOURCE_DIR}"

        )

set_source_files_properties(${BOARD_SOURCE_FILES_PROPERTIES})

target_link_libraries(${PROJECT_NAME} module-os module-drivers ${TARGET_LIBRARIES})

# Host target configuration(mainly used for unit testing)

if (DEFINED BUILD_UNIT_TESTS)
    add_subdirectory(tests)
endif ()

