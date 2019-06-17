cmake_minimum_required(VERSION 3.14)

project(module-apps VERSION 1.0
        DESCRIPTION "Library with all applications.")
        
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

set(CMAKE_CXX_STANDARD 14)

message("PROJECT TARGET: ${PROJECT_TARGET}")
if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
	set(CMAKE_CXX_STANDARD 17)
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

set( SOURCES "Application.cpp" )

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

message("${PROJECT_NAME}: add_subdirectory application-clock")
add_subdirectory( application-clock )
add_subdirectory( application-calendar )
add_subdirectory( application-test )
add_subdirectory( application-viewer )

include_directories( ../module-sys )
include_directories( ../module-utils )
include_directories( ../module-services )
include_directories( ../module-gui )
include_directories( ../module-os )
include_directories( ../module-bsp )

target_link_libraries(${PROJECT_NAME} PUBLIC module-bsp module-os module-sys module-utils module-gui module-services ${TARGET_LIBRARIES} )

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})



target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
)


target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        )

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

endif()

