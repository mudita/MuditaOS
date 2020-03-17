cmake_minimum_required(VERSION 3.14)

project(module-sys VERSION 1.0
        DESCRIPTION "Core module library")

set(SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Bus.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Worker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Message.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/SystemManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/PowerManager.cpp

        )

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})


target_link_libraries(${PROJECT_NAME} PUBLIC module-os)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})



target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Host target configuration(mainly used for unit testing)

message(WARNING "Tests for ${PROJECT_NAME} need update - disabled: https://appnroll.atlassian.net/browse/EGD-2988")
#add_subdirectory(tests)

