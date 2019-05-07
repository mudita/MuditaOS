cmake_minimum_required(VERSION 3.14)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(PurePhone)

include(env.cmake)

message("PROJECT_TARGET: ${PROJECT_TARGET}")
message("TARGET_SOURCES: ${TARGET_SOURCES}")
message("TARGET_COMPILE_DEFINITIONS: ${TARGET_COMPILE_OPTIONS}")
message("TARGET_LIBRARIES: ${TARGET_LIBRARIES}")
message("TARGET_LINKER_FLAGS: ${TARGET_LINKER_FLAGS}")

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME} "")


# If build type not passed assume it is Debug
if(CMAKE_BUILD_TYPE STREQUAL  "")
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "CMAKE_BUILD_TYPE not set, assuming debug build...")
endif()

if(CMAKE_BUILD_TYPE STREQUAL  "Debug")
    message(STATUS "Build type: Debug")
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DDEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL  "Release")
    message(STATUS "Build type: Release")
    target_compile_options(${PROJECT_NAME} PUBLIC -O0)
endif()

if(CMAKE_BUILD_TYPE STREQUAL  "RelWithDebInfo")
    message(STATUS "Build type: RelWithDebInfo")
    target_compile_options(${PROJECT_NAME} PUBLIC -O0)
    target_compile_options(${PROJECT_NAME} PUBLIC -g3)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -DDEBUG)
endif()

set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")


set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building" )
message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")

target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
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
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_HAVE_SQLITE_CONFIG_H)
target_compile_definitions(${PROJECT_NAME} PUBLIC -D_GNU_SOURCE)


target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:-std=gnu11>)
target_compile_options(${PROJECT_NAME} PUBLIC -fsingle-precision-constant)
target_compile_options(${PROJECT_NAME} PUBLIC -ffunction-sections)
target_compile_options(${PROJECT_NAME} PUBLIC -fdata-sections)
target_compile_options(${PROJECT_NAME} PUBLIC -MMD )
target_compile_options(${PROJECT_NAME} PUBLIC -MP)
target_compile_options(${PROJECT_NAME} PUBLIC -fno-builtin)

target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})


# Warnings related flags
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>)

# Flags used only for .cpp files
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_noexcept)
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>)
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>)
target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)




target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${TARGET_SOURCES})
target_sources(${CMAKE_PROJECT_NAME}
        PRIVATE

        source/main.cpp

        )

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${TARGET_DIR_INCLUDES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/config)

message("${PROJECT_NAME}: add_subdirectory module-sys")
add_subdirectory(module-sys)
include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/module-sys" )


message("${PROJECT_NAME}: add_subdirectory module-gui")
add_subdirectory(module-gui)
include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/module-gui" )

message("${PROJECT_NAME}: add_subdirectory module-utils")
add_subdirectory(module-utils)
include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/module-utils" )


set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".elf")

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS "-Xlinker -Map=${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map ")

target_link_libraries(${PROJECT_NAME} ${TARGET_LIBRARIES} module-sys  ${LIBRARY_GUI})