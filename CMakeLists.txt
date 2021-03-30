cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project(PurePhone)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(Colours)
include(CCacheConfig)
include(ProjectConfig)
include(ModuleConfig)
include(module-lwip/lwip-includes.cmake)
include(SerialPort)
include(CopyGdbInit)
include(Utils)

message("PROJECT_TARGET: ${PROJECT_TARGET}")
message("TARGET_SOURCES: ${TARGET_SOURCES}")
message("TARGET_COMPILE_DEFINITIONS: ${TARGET_COMPILE_OPTIONS}")
message("TARGET_LIBRARIES: ${TARGET_LIBRARIES}")
message("TARGET_LINKER_FLAGS: ${TARGET_LINKER_FLAGS}")

string(REPLACE "TARGET_" "" PROJECT_TARGET_NAME ${PROJECT_TARGET})
string(TOLOWER "${PROJECT_TARGET_NAME}" PROJECT_TARGET_NAME)
message("Project target name: ${PROJECT_TARGET_NAME}")

add_executable(${PROJECT_NAME} "" )

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
                     -fno-diagnostics-color
                     # warning flags
                     -Wall -Wextra -Werror -Wno-unused-parameter -Wno-deprecated-declarations)

option (GENERATE_STACK_USAGE "Generate stack usage report" OFF)
if (GENERATE_STACK_USAGE)
    add_compile_options (-fstack-usage)
endif ()

target_compile_features(${PROJECT_NAME} PUBLIC
        ${TARGET_COMPILE_FEATURES})


target_compile_options(${PROJECT_NAME} PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>)
add_compile_options("-Wno-error=deprecated-declarations")
target_compile_options(${PROJECT_NAME} PUBLIC "-Wno-error=deprecated-declarations")

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
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR})

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
     add_subdirectory(board/linux/libiosyscalls)
endif()
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
        module-apps
        service-desktop
        service-bluetooth
        service-lwip
        ${LWIP_LIBRARIES}
        )

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    target_link_libraries(${PROJECT_NAME} iosyscalls)
endif()

include(tools/clang-tidy.cmake)

set_source_files_properties(source/main.cpp PROPERTIES COMPILE_DEFINITIONS "${ENABLED_APPS_DEFINES}")

target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

add_subdirectory(host-tools)


if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    set(HEX_FILE ${CMAKE_PROJECT_NAME}.hex)
    set(BIN_FILE ${CMAKE_BINARY_DIR}/sys/current/boot.bin)

    add_custom_command(
        COMMENT "Generate boot.bin"
        OUTPUT ${BIN_FILE}
        DEPENDS ${CMAKE_PROJECT_NAME}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${BIN_FILE}
        )

    add_custom_command(
        COMMENT "Generate ${CMAKE_PROJECT_NAME}.hex"
        OUTPUT ${CMAKE_BINARY_DIR}/${HEX_FILE}
        DEPENDS ${CMAKE_PROJECT_NAME}
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${HEX_FILE}
        )

    add_custom_target(
        ${HEX_FILE}-target ALL
        DEPENDS ${CMAKE_BINARY_DIR}/${HEX_FILE}
        )

    # download the bootloader
    set(ECOBOOT_FILE ecoboot.bin)
    set(ECOBOOT_DOWNLOAD_LOG ${CMAKE_BINARY_DIR}/download_info.txt)

    if(DEFINED ENV{ASSETS_LOGIN} AND DEFINED ENV{ASSETS_TOKEN})
        message("ecooboot download with tokens")
        add_custom_command(OUTPUT ${ECOBOOT_FILE}
            COMMAND ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            -l $ENV{ASSETS_LOGIN}
            -t $ENV{ASSETS_TOKEN}
            -w ${CMAKE_BINARY_DIR} ecoboot download
            > ${ECOBOOT_DOWNLOAD_LOG}
            COMMENT "Downloading ${ECOBOOT_FILE}"
            BYPRODUCTS ${ECOBOOT_DOWNLOAD_LOG}
            )
    else()
        message("ecoboot download with git")
        add_custom_command(OUTPUT ${ECOBOOT_FILE}
            COMMAND ${CMAKE_SOURCE_DIR}/tools/download_asset.py
            -w ${CMAKE_BINARY_DIR} ecoboot download
            > ${ECOBOOT_DOWNLOAD_LOG}
            COMMENT "Downloading ${ECOBOOT_FILE}"
            BYPRODUCTS ${ECOBOOT_DOWNLOAD_LOG}
            )
    endif()

    add_custom_target(${ECOBOOT_FILE}-target DEPENDS ${ECOBOOT_FILE})

    # generate version.json file (runs CMake script at build time)
    set(VERSION_JSON_FILE version.json)
    add_custom_command(OUTPUT ${VERSION_JSON_FILE}
        COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR=${CMAKE_SOURCE_DIR}
        -DECOBOOT_DOWNLOAD_LOG=${ECOBOOT_DOWNLOAD_LOG}
        -B ${CMAKE_BINARY_DIR}
        -P ${CMAKE_SOURCE_DIR}/cmake/modules/GenUpdateVersionJson.cmake
        DEPENDS ${ECOBOOT_FILE}-target ${ECOBOOT_DOWNLOAD_LOG}
        )

    add_custom_target(${VERSION_JSON_FILE}-target DEPENDS ${VERSION_JSON_FILE})

    multicomp_install(PROGRAMS ${CMAKE_BINARY_DIR}/${ECOBOOT_FILE} DESTINATION "./" 
        COMPONENTS Standalone Update)
    multicomp_install(FILES ${CMAKE_BINARY_DIR}/${VERSION_JSON_FILE} DESTINATION "./" 
        COMPONENTS Standalone Update)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-debug --strip-unneeded $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
        )
endif()

install(
    FILES ${PROJECT_SOURCE_DIR}/changelog.md
    DESTINATION "./"
    COMPONENT Standalone
    )

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

include(DiskImage)

message_serial_status()

option (BUILD_DOC_WITH_ALL "Build documentation" OFF)
include (GenDoxygenDoc)

message("SRC DIR: ${CMAKE_SOURCE_DIR}")

include(Version)
configure_file(
    ${SRC_DIR}/source/version.hpp.template
    ${CMAKE_BINARY_DIR}/source/version.hpp
    )
add_custom_target(
    version ALL
    COMMAND ${CMAKE_COMMAND}
    -DSRC_DIR=${CMAKE_SOURCE_DIR}
    -B ${CMAKE_BINARY_DIR}
    -P ${CMAKE_SOURCE_DIR}/cmake/modules/GenVersionHpp.cmake
    COMMENT
        "Generating version info"
    )
add_dependencies(${PROJECT_NAME} version)

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    set(CPACK_SYSTEM_NAME "Linux")
    set(CPACK_COMPONENTS_ALL Standalone)
else()
    set(CPACK_SYSTEM_NAME "RT1051")
    set(CPACK_COMPONENTS_ALL Standalone Update)
endif()
set(CPACK_PACKAGE_VENDOR "Mudita")
set(CPACK_PACKAGE_NAME "PurePhone")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://mudita.com/products/pure/")
set(CPACK_TOPLEVEL_TAG ${CPACK_SYSTEM_NAME})
# the CPACK_PACKAGE_FILE_NAME variable will be reset after include(CPack) hence a copy
set(PACKAGE_COMMON_NAME ${CPACK_PACKAGE_NAME}-${CMAKE_PROJECT_VERSION}-${CPACK_TOPLEVEL_TAG})
set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_COMMON_NAME})
# setting this will CPack prevent from additing the default 'package' target
set(CPACK_OUTPUT_CONFIG_FILE ${CMAKE_BINARY_DIR}/PackageConfig.cmake)
set(CPACK_GENERATOR "External")
set(CPACK_COMPONENTS_GROUPING IGNORE)
set(CPACK_EXTERNAL_ENABLE_STAGING TRUE)
set(PACKAGE_STAGING_DIRECTORY ${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_TOPLEVEL_TAG}/${CPACK_GENERATOR}/${CPACK_PACKAGE_FILE_NAME})
message(PACKAGE_STAGING_DIRECTORY ${CMAKE_BINARY_DIR}/_CPack_Packages/${CPACK_TOPLEVEL_TAG}/${CPACK_GENERATOR}/${CPACK_PACKAGE_FILE_NAME})
message("CPACK_TOPLEVEL_TAG:'${CPACK_TOPLEVEL_TAG}'")
message("CPACK_GENERATOR:'${CPACK_GENERATOR}'")
message("CPACK_PACKAGE_FILE_NAME:'${CPACK_PACKAGE_FILE_NAME}'")

include(CPack)

if (${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    multicomp_install(
        TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION "./"
        COMPONENTS Standalone Update
        )
    add_custom_target(package-standalone-staged
        COMMAND ${CMAKE_CPACK_COMMAND}
        -C $<CONFIGURATION>
        --config ${CPACK_OUTPUT_CONFIG_FILE}
        DEPENDS ${CMAKE_PROJECT_NAME} service_renderer
    )
    set(PACKAGE_STANDALONE_FILE_NAME ${PACKAGE_COMMON_NAME}-Standalone.tar.gz)
    set(PACKAGE_STANDALONE_MIME "application/x-compressed-tar")

    add_custom_target(package-standalone
        COMMAND tar
        -czf ${CMAKE_BINARY_DIR}/${PACKAGE_STANDALONE_FILE_NAME}
        -C ${PACKAGE_STAGING_DIRECTORY}/Standalone "."
        DEPENDS package-standalone-staged
        )
endif()

if (${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    set(PACKAGE_STANDALONE_MIME "application/x-xz")
    set(PACKAGE_STANDALONE_FILE_NAME ${PACKAGE_COMMON_NAME}-image.tar.xz)
    add_custom_target(package-standalone
        COMMAND tar -ScJf ${PACKAGE_STANDALONE_FILE_NAME} ${DISK_IMAGE_NAME}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${BIN_FILE} ${ECOBOOT_FILE}-target ${VERSION_JSON_FILE}-target disk_image
        )

    set(PACKAGE_UPDATE_FILE_NAME ${PACKAGE_COMMON_NAME}-Update.tar)
    set(PACKAGE_UPDATE_MIME "application/x-tar")
    set(UPDATE_STAGING_DIRECTORY ${PACKAGE_STAGING_DIRECTORY}/Update)
    add_custom_target(package-update-staged
        COMMAND ${CMAKE_CPACK_COMMAND}
        -C $<CONFIGURATION>
        --config ${CPACK_OUTPUT_CONFIG_FILE}
        DEPENDS  ${BIN_FILE} ${ECOBOOT_FILE}-target ${VERSION_JSON_FILE}-target
        )
    add_custom_target(package-update-checksums
        COMMAND rhash
        -u checksums.txt
        -r .
        WORKING_DIRECTORY ${UPDATE_STAGING_DIRECTORY}
        DEPENDS package-update-staged
        )

    
    add_custom_target(package-update
        COMMAND tar
        -cf ${CMAKE_BINARY_DIR}/${PACKAGE_UPDATE_FILE_NAME}
        -C ${PACKAGE_STAGING_DIRECTORY}/Update "."
        DEPENDS  ${ECOBOOT_FILE}-target ${VERSION_JSON_FILE}-target PurePhone package-update-staged package-update-checksums 
        )
endif()




if (NOT "$ENV{GITHUB_WORKSPACE}" STREQUAL "")
    message("set-output name=package-standalone::${PACKAGE_STANDALONE_FILE_NAME}")
    message("::set-output name=package-standalone::${PACKAGE_STANDALONE_FILE_NAME}")
    message("set-output name=standalone-mime_type::${PACKAGE_STANDALONE_MIME}")
    message("::set-output name=standalone-mime_type::${PACKAGE_STANDALONE_MIME}")

    message("set-output name=package-update::${PACKAGE_UPDATE_FILE_NAME}")
    message("::set-output name=package-update::${PACKAGE_UPDATE_FILE_NAME}")
    message("set-output name=update-mime_type::${PACKAGE_UPDATE_MIME}")
    message("::set-output name=update-mime_type::${PACKAGE_UPDATE_MIME}")

endif()

