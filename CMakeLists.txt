cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
include(env.cmake)
project(PurePhone)

include(config/ProjectConfig.cmake)
include(config/Version.cmake)
include(module-lwip/lwip-includes.cmake)

message("PROJECT_TARGET: ${PROJECT_TARGET}")
message("TARGET_SOURCES: ${TARGET_SOURCES}")
message("TARGET_COMPILE_DEFINITIONS: ${TARGET_COMPILE_OPTIONS}")
message("TARGET_LIBRARIES: ${TARGET_LIBRARIES}")
message("TARGET_LINKER_FLAGS: ${TARGET_LINKER_FLAGS}")

add_executable(${PROJECT_NAME} "")


# If build type not passed assume it is Debug
if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "CMAKE_BUILD_TYPE not set, assuming debug build...")
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Build type: Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DDEBUG)
    #target_compile_options(${PROJECT_NAME} PUBLIC -O3)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Build type: Release")
    target_compile_options(${PROJECT_NAME} PUBLIC -O0)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "Build type: RelWithDebInfo")
    target_compile_options(${PROJECT_NAME} PUBLIC -O0)
    target_compile_options(${PROJECT_NAME} PUBLIC -g3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DDEBUG)
endif ()

set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_LIB_DIRECTORY})


set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building")
message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")

target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${SERIAL_PORT_PATH})
target_compile_definitions(${PROJECT_NAME} PUBLIC -DFSL_RTOS_FREE_RTOS)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DUSB_STACK_FREERTOS)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DDEBUG_CONSOLE_TRANSFER_NON_BLOCKING)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DFSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__STARTUP_INITIALIZE_NONCACHEDATA)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DSDK_OS_FREE_RTOS)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DXIP_EXTERNAL_FLASH=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DXIP_BOOT_HEADER_ENABLE=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DXIP_BOOT_HEADER_DCD_ENABLE=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DCPU_MIMXRT1051DVL6B_cm7)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DCPU_MIMXRT1051DVL6B)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DSDK_DEBUGCONSOLE=0)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__MCUXPRESSO)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__USE_CMSIS)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D__NEWLIB__)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DSKIP_SYSCLK_INIT)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_GNU_SOURCE)


target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:-std=gnu11>)
target_compile_options(${PROJECT_NAME} PUBLIC -fsingle-precision-constant)
target_compile_options(${PROJECT_NAME} PUBLIC -ffunction-sections)
target_compile_options(${PROJECT_NAME} PUBLIC -fdata-sections)
target_compile_options(${PROJECT_NAME} PUBLIC -MMD)
target_compile_options(${PROJECT_NAME} PUBLIC -MP)
target_compile_options(${PROJECT_NAME} PUBLIC -fno-builtin)

target_compile_features(${PROJECT_NAME} PUBLIC
        ${TARGET_COMPILE_FEATURES})


# Warnings related flags
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>)

target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${TARGET_SOURCES})
target_sources(${CMAKE_PROJECT_NAME}
        PRIVATE

        source/main.cpp
        PUBLIC
        source/MessageType.hpp

        )

set(PROJECT_INCLUDES

        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/config


        CACHE INTERNAL "")

target_include_directories(${PROJECT_NAME} PUBLIC ${TARGET_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})


message("${PROJECT_NAME}: add_subdirectory module-sys")
add_subdirectory(module-sys)

message("${PROJECT_NAME}: add_subdirectory module-utils")
add_subdirectory(module-utils)

message("${PROJECT_NAME}: add_subdirectory module-os")
add_subdirectory(module-os)

message("${PROJECT_NAME}: add_subdirectory module-bsp")
add_subdirectory(module-bsp)

message("${PROJECT_NAME}: add_subdirectory module-vfs")
add_subdirectory(module-vfs)

message("${PROJECT_NAME}: add_subdirectory module-gui")
add_subdirectory(module-gui)

message("${PROJECT_NAME}: add_subdirectory module-db")
add_subdirectory(module-db)

message("${PROJECT_NAME}: add_subdirectory module-cellular")
add_subdirectory(module-cellular)

message("${PROJECT_NAME}: add_subdirectory module-audio")
add_subdirectory(module-audio)

message("${PROJECT_NAME}: add_subdirectory module-services")
add_subdirectory(module-services)

message("${PROJECT_NAME}: add_subdirectory module-apps")
add_subdirectory(module-apps)

message("${PROJECT_NAME}: add_subdirectory module-bluetooth")
add_subdirectory(module-bluetooth)

message("${PROJECT_NAME}: add_subdirectory module-lwip")
add_subdirectory(module-lwip)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".elf")

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS "-Xlinker -Map=${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map ")

# M.P: Please don't change the order of modules in the first group ("module-bsp" - "module-utils")
# They have to be arranged in specific order because of circular dependencies that need to be correctly resolved
# In case of doubt please contact me: mateusz.piesta@mudita.com
target_link_libraries(${PROJECT_NAME}
        module-bsp
        module-os
        module-bsp
        module-utils
        module-vfs
        module-utils


        module-sys
        module-cellular
        module-audio
        module-db
        module-gui
        module-services
        module-apps
        module-bluetooth
        ${LWIP_LIBRARIES}
        module-lwip
        ${TARGET_LIBRARIES}
        )


target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

set(HEX_FILE ${PROJECT_BUILD_DIRECTORY}/${CMAKE_PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BUILD_DIRECTORY}/boot.bin)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${BIN_FILE}
        )

