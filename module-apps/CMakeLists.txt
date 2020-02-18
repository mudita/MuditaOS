cmake_minimum_required(VERSION 3.14)

project(module-apps VERSION 1.0
        DESCRIPTION "Library with all applications.")

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

set( SOURCES "Application.cpp" "windows/AppWindow.cpp" "windows/OptionWindow.cpp" "windows/Dialog.cpp" )

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

add_subdirectory( application-clock )
add_subdirectory( application-calendar )
add_subdirectory( application-viewer )
add_subdirectory( application-desktop )
add_subdirectory( application-settings )
add_subdirectory( application-call )
add_subdirectory( application-notes )
add_subdirectory( application-phonebook )
add_subdirectory( application-messages )
add_subdirectory( application-calllog )
add_subdirectory( application-special-input )

include_directories( ../module-sys )
include_directories( ../module-utils )
include_directories( ../module-services )
include_directories( ../module-gui )
include_directories( ../module-os )
include_directories( ../module-bsp )
include_directories( ../module-db )

target_link_libraries(${PROJECT_NAME} PUBLIC module-bsp module-os module-sys module-utils module-gui module-services module-db ${TARGET_LIBRARIES} )

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/messages
        ${CMAKE_CURRENT_SOURCE_DIR}/windows
)


target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        )

target_compile_options(${PROJECT_NAME}

        PUBLIC

        -Wall

        $<$<COMPILE_LANGUAGE:C>:-std=gnu11>
        $<$<COMPILE_LANGUAGE:C>:-Wno-discarded-qualifiers>

        $<$<COMPILE_LANGUAGE:CXX>:-fno-non-call-exceptions>
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-literal-suffix>

)

target_compile_features(${PROJECT_NAME} PUBLIC

        cxx_std_14
)

# Host target configuration(mainly used for unit testing)
if(DEFINED BUILD_UNIT_TESTS)

endif()

