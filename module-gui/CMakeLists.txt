cmake_minimum_required(VERSION 3.12)


if(TARGET module-gui)
    return() # The project has already been built.
endif()


project(module-gui VERSION 1.0
        DESCRIPTION "GUI library dedicated for pure phone.")

set(CMAKE_CXX_STANDARD 14)

# Get the current working branch
execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the latest abbreviated commit hash of the working branch
execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(MOD_VERSION ${PROJECT_VERSION})

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/version.h.in
        ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}-version.h
)

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

message("PROJECT TARGET: ${PROJECT_TARGET}")
if( NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
else()
	set(CMAKE_CXX_STANDARD 17)
    include(targets/Target_Linux.cmake)
endif()


message("${PROJECT_NAME}: add_subdirectory module-utils")
add_subdirectory(module-utils)
#add_subdirectory(module-vfs)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})
include(gui/CMakeLists.txt)
include(gui/core/CMakeLists.txt)
include(gui/widgets/CMakeLists.txt)

target_link_libraries(${PROJECT_NAME} PUBLIC module-utils ${TARGET_LIBRARIES} )
#vfs disabled
#target_link_libraries(${PROJECT_NAME} PUBLIC module-utils module-vfs ${TARGET_LIBRARIES} )

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES} )
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})


target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
)


target_compile_definitions(${PROJECT_NAME} PUBLIC )

target_compile_options(${PROJECT_NAME}

        PUBLIC

        -Wall

        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>

        $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>

)

target_compile_features(${PROJECT_NAME} PUBLIC

        cxx_std_14
        cxx_noexcept
)

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

	add_subdirectory( host_renderer )
	add_subdirectory( test )

endif()
