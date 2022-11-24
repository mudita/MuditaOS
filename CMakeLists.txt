cmake_minimum_required(VERSION 3.19.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(MUDITA_CACHE_DIR "~/.mudita/" CACHE STRING "cache directory for our downloaded assets")

project(PureOS)
enable_language(C CXX ASM)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(Product)
include(Board)

set(SYSROOT_PATH ${CMAKE_CURRENT_BINARY_DIR}/sysroot)
set(SYSROOT_BIN_PATH ${SYSROOT_PATH}/system_a/bin)

validate_product_selection()
include(products/${PRODUCT}/config.cmake)
select_board()

include(Colours)
include(CCacheConfig)
include(ProjectConfig)
include(ModuleConfig)
include(SerialPort)
include(CopyGdbInit)
include(Utils)
include(ModuleUtils)
include(DiskImage)
include(AddPackage)
include(AutoModuleOption)
include(AddDirectories)
include(AddDatabases)
include(FetchContent)
include(AddScripts)

message("Selected product: ${PRODUCT}")
message("Selected board:   ${BOARD}")
message("Board revision:   ${BOARD_REVISION}")
message("PROJECT_TARGET:   ${PROJECT_TARGET}")
message("Assets version:   ${ASSETS_TYPE}")
message("Version:          ${OS_VERSION_MAJOR}.${OS_VERSION_MINOR}.${OS_VERSION_PATCH}-${OS_VERSION_LABEL}")

string(REPLACE "TARGET_" "" PROJECT_TARGET_NAME ${PROJECT_TARGET})
string(TOLOWER "${PROJECT_TARGET_NAME}" PROJECT_TARGET_NAME)
message("Project target name: ${PROJECT_TARGET_NAME}")
if (NOT ${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    set(ENABLE_TESTS ON)
else()
    add_compile_options(-include ${CMAKE_SOURCE_DIR}/board/rt1051/gthr.h)
endif()

if (${ENABLE_TESTS})
    enable_testing()
    add_custom_target(check ${CMAKE_CTEST_COMMAND} -V DEPENDS create_test_product_databases create_test_databases_common)
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

add_compile_options(
    $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
    -fsingle-precision-constant
    -ffunction-sections
    -fdata-sections
    -MMD
    -MP
    -fno-builtin
    # warning flags
    -Wall -Wextra -Werror -Wno-unused-parameter -Wno-deprecated-declarations)

include_directories(${CMAKE_SOURCE_DIR})

option (GENERATE_STACK_USAGE "Generate stack usage report" OFF)
if (GENERATE_STACK_USAGE)
    add_compile_options (-fstack-usage)
endif ()

option (ENABLE_SECURE_BOOT "Build signed binary for Secure Boot" OFF)
set(SIGN_CLIENT_PATH "${CMAKE_SOURCE_DIR}/../sign_server/key_client" CACHE PATH "signclient.py path")
set(SERVER "https://172.17.0.1:4430" CACHE STRING "sign server address")

if (NOT ENABLE_SECURE_BOOT)
    # Only enable the build-time boot header in a non-secure configuration,
    # a header for Secure Boot is added post-build when signing the binary
    add_compile_definitions(
        XIP_BOOT_HEADER_ENABLE=1
        XIP_BOOT_HEADER_DCD_ENABLE=0
        )
endif ()

FetchContent_Declare(
        pure-core
        GIT_REPOSITORY https://github.com/mudita/pure-core.git
)
FetchContent_MakeAvailable(pure-core)

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
add_subdirectory(image)
add_subdirectory(host-tools)
add_subdirectory(third-party)

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

# Create common system directories
add_directories(
        TARGET system_directories_common
        PREFIX ${SYSROOT_PATH}/system_a
        DEPENDS assets
        DIRECTORIES log assets crash_dumps data bin db scripts
)

# Create and initialize common databases
add_databases_target(
        TARGET create_databases_common
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/module-db/databases/scripts
        DEST_DIR ${SYSROOT_PATH}/system_a/db
        DEVEL ${WITH_DEVELOPMENT_FEATURES}
)

# Create and initialize product-specific databases
add_databases_target(
        TARGET create_product_databases
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/products/${PRODUCT}/services/db/databases/scripts
        DEST_DIR ${SYSROOT_PATH}/system_a/db
        DEVEL ${WITH_DEVELOPMENT_FEATURES}
        DEPENDS create_databases_common
)
# Install scripts
add_scripts_target(
        TARGET install_scripts
        PRODUCT ${PRODUCT}
        DEST_DIR ${SYSROOT_PATH}/system_a/scripts
)

# Create and initialize common databases for tests
add_databases_target(
        TARGET create_test_databases_common
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/module-db/databases/scripts
        DEST_DIR ${CMAKE_BINARY_DIR}/test-sysroot/system_a/db
        DEVEL ${WITH_DEVELOPMENT_FEATURES}
)

# Create and initialize product-specific databases for tests
add_databases_target(
        TARGET create_test_product_databases
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/products/${PRODUCT}/services/db/databases/scripts
        DEST_DIR ${CMAKE_BINARY_DIR}/test-sysroot/system_a/db
        DEVEL ${WITH_DEVELOPMENT_FEATURES}
        DEPENDS create_databases_common
)

add_library(version-header INTERFACE)
target_include_directories(version-header INTERFACE $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/source/include>)
add_dependencies(version-header version)

add_subdirectory(products)
