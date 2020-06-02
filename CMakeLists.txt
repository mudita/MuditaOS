cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(PurePhone)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")

include(Colours)
include(CCacheConfig)
include(ProjectConfig)
include(Version)
include(ModuleConfig)
include(module-lwip/lwip-includes.cmake)
include(SerialPort)
include(CopyGdbInit)

message("PROJECT_TARGET: ${PROJECT_TARGET}")
message("TARGET_SOURCES: ${TARGET_SOURCES}")
message("TARGET_COMPILE_DEFINITIONS: ${TARGET_COMPILE_OPTIONS}")
message("TARGET_LIBRARIES: ${TARGET_LIBRARIES}")
message("TARGET_LINKER_FLAGS: ${TARGET_LINKER_FLAGS}")

add_executable(${PROJECT_NAME} "")

enable_testing()
add_custom_target(check ${CMAKE_CTEST_COMMAND} -V)
add_subdirectory(test)

# setting build flags
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O2 -DDEBUG" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELEASE "-O2" CACHE STRING "")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb3 -DDEBUG" CACHE STRING "")

set(TOOLCHAIN_PATH "" CACHE STRING "Path to coolchain directory")

target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_LIB_DIRECTORY})

message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
message("Setting CMAKE_BINARY_DIR to ${CMAKE_BINARY_DIR}")
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS}
                ${TARGET_COMPILE_DEFINITIONS}
                ${PROJECT_TARGET}
                )

define_serial(${PROJECT_NAME})

add_compile_definitions(FSL_RTOS_FREE_RTOS
                USB_STACK_FREERTOS
                DEBUG_CONSOLE_TRANSFER_NON_BLOCKING
                FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1
                __STARTUP_INITIALIZE_NONCACHEDATA
                SDK_OS_FREE_RTOS
                XIP_EXTERNAL_FLASH=0
                XIP_BOOT_HEADER_ENABLE=1
                XIP_BOOT_HEADER_DCD_ENABLE=0
                CPU_MIMXRT1051DVL6B_cm7
                CPU_MIMXRT1051DVL6B
                SDK_DEBUGCONSOLE=0
                __MCUXPRESSO
                __USE_CMSIS
                __NEWLIB__
                SKIP_SYSCLK_INIT
                _GNU_SOURCE)

add_compile_options( ${TARGET_COMPILE_OPTIONS}
                     $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
                     -fsingle-precision-constant
                     -ffunction-sections
                     -fdata-sections
                     -MMD
                     -MP
                     -fno-builtin
                     # warning flags
                     -Wall -Wextra -Werror -Wno-unused-parameter)

target_compile_features(${PROJECT_NAME} PUBLIC
        ${TARGET_COMPILE_FEATURES})


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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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

add_subdirectory(image)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".elf")

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS "-Xlinker -Map=${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map ")

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

set(HEX_FILE ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_BINARY_DIR}/boot.bin)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${BIN_FILE}
        )


if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        )
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} --only-keep-debug
                $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
                $<TARGET_FILE:${CMAKE_PROJECT_NAME}>.debug
        COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:${CMAKE_PROJECT_NAME}>.debug
                $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        )
endif()

message_serial_status()

option (BUILD_DOC_WITH_ALL "Build documentation" OFF)

# check if Doxygen is installed
find_package (Doxygen COMPONENTS dot)
if (DOXYGEN_FOUND)
        # set custom command scope
        unset (_DOC_ALL)
        if (BUILD_DOC_WITH_ALL)
                set (_DOC_ALL ALL)
        endif ()

        # set input and output files
        set (DOXYGEN_IN ${PROJECT_SOURCE_DIR}/doc/Doxyfile.in)
        set (DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

        # request to configure the file
        configure_file (${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        # add target for doxygen documentation
        add_custom_target(doc ${_DOC_ALL}
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM
        )
else ()
        if (BUILD_DOC_WITH_ALL)
                message (SEND_ERROR ${BoldRed} "Could not find doxygen with dot to build code documentation." ${ColourReset})
        else()
                message (WARNING ${BoldYellow} "Install doxygen and graphviz to be able to build code documentation" ${ColourReset})
        endif()
endif ()
