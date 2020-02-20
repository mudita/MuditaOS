cmake_minimum_required(VERSION 3.12)

project(module-vfs VERSION 1.0
        DESCRIPTION "VFS module library")

set(SOURCES "")

if(NOT ${PROJECT_TARGET}  STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
endif()

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})


# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})

target_compile_definitions(${PROJECT_NAME}

        PUBLIC
)

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}


)

target_link_libraries(${PROJECT_NAME} ${TARGET_LIBRARIES} module-os module-bsp module-utils)

if(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    target_link_libraries(${PROJECT_NAME} -lstdc++fs)
endif()

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)
    add_subdirectory(tests)
endif()

