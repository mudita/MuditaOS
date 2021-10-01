cmake_minimum_required(VERSION 3.19.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(PureOS)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(Colours)
include(CCacheConfig)
include(ProjectConfig)
include(ModuleConfig)
include(module-lwip/lwip-includes.cmake)
include(SerialPort)
include(CopyGdbInit)
include(Utils)
include(ModuleUtils)
include(FetchBootloader)
include(FetchPureUpdater)
include(DiskImage)
include(AddPackage)
include(Product)

validate_product_selection()

message("Selected product: ${PRODUCT}")
message("PROJECT_TARGET: ${PROJECT_TARGET}")
message("TARGET_COMPILE_DEFINITIONS: ${TARGET_COMPILE_OPTIONS}")
message("TARGET_LIBRARIES: ${TARGET_LIBRARIES}")
message("TARGET_LINKER_FLAGS: ${TARGET_LINKER_FLAGS}")
message("Version:          ${OS_VERSION_MAJOR}.${OS_VERSION_MINOR}.${OS_VERSION_PATCH}-${OS_VERSION_LABEL}")

string(REPLACE "TARGET_" "" PROJECT_TARGET_NAME ${PROJECT_TARGET})
string(TOLOWER "${PROJECT_TARGET_NAME}" PROJECT_TARGET_NAME)
message("Project target name: ${PROJECT_TARGET_NAME}")
if (NOT ${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    set(ENABLE_TESTS ON)
endif()

if (${ENABLE_TESTS})
    enable_testing()
    add_custom_target(check ${CMAKE_CTEST_COMMAND} -V)
    add_subdirectory(test)
    include(PureCoverage)
endif ()

# setting build flags
if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -ggdb3 -DNDEBUG")

    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb3 -DNDEBUG")
else()
    set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -DDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -ggdb3 -DNDEBUG")

    set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb3 -DNDEBUG")

endif()

set(TOOLCHAIN_PATH "" CACHE STRING "Path to toolchain directory")

add_compile_definitions(FSL_RTOS_FREE_RTOS
    PLATFORM_${PROJECT_TARGET_NAME}
    USB_STACK_FREERTOS
    DEBUG_CONSOLE_TRANSFER_NON_BLOCKING
    FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1
    __STARTUP_INITIALIZE_NONCACHEDATA
    SDK_OS_FREE_RTOS
    XIP_EXTERNAL_FLASH=0
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
    -fno-diagnostics-color
    # warning flags
    -Wall -Wextra -Werror -Wno-unused-parameter -Wno-deprecated-declarations)

include_directories(${CMAKE_SOURCE_DIR})

option (GENERATE_STACK_USAGE "Generate stack usage report" OFF)
if (GENERATE_STACK_USAGE)
    add_compile_options (-fstack-usage)
endif ()

include(SecureBoot)

if (NOT ENABLE_SECURE_BOOT)
    # Only enable the build-time boot header in a non-secure configuration,
    # a header for Secure Boot is added post-build when signing the binary
    add_compile_definitions(
        XIP_BOOT_HEADER_ENABLE=1
        XIP_BOOT_HEADER_DCD_ENABLE=0
        )
endif ()

add_subdirectory(third-party)
add_subdirectory(board)
add_subdirectory(source)
add_subdirectory(module-platform)
add_subdirectory(module-sys)
add_subdirectory(module-utils)
add_subdirectory(module-os)
add_subdirectory(module-bsp)
add_subdirectory(module-vfs)
add_subdirectory(module-gui)
add_subdirectory(module-db)
add_subdirectory(module-cellular)
add_subdirectory(module-audio)
add_subdirectory(module-services)
add_subdirectory(module-apps)
add_subdirectory(module-bluetooth)
add_subdirectory(module-lwip)
add_subdirectory(image)
add_subdirectory(host-tools)

include(tools/clang-tidy.cmake)

install(
    FILES ${PROJECT_SOURCE_DIR}/changelog.md
    DESTINATION "./"
    COMPONENT Standalone
    )

option (BUILD_DOC_WITH_ALL "Build documentation" OFF)
include (GenDoxygenDoc)

include(Version)

add_custom_target(
    version ALL
    COMMAND ${CMAKE_COMMAND}
    -DSRC_DIR=${CMAKE_SOURCE_DIR}
    -B ${CMAKE_BINARY_DIR}
    -DOS_VERSION_MAJOR=${OS_VERSION_MAJOR}
    -DOS_VERSION_MINOR=${OS_VERSION_MINOR}
    -DOS_VERSION_PATCH=${OS_VERSION_PATCH}
    -DOS_VERSION_LABEL=${OS_VERSION_LABEL}
    -P ${CMAKE_SOURCE_DIR}/cmake/modules/GenVersionHpp.cmake
    COMMENT
        "Generating version info"
    )

fetch_ecoboot()
fetch_updater()

add_subdirectory(products)

