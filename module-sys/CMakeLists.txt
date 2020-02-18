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


target_compile_definitions(${PROJECT_NAME}

        PUBLIC
        )

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

       add_executable(unittest_${PROJECT_NAME}
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/Test1/TestService1.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/Test1/CommandsService.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/Test1/DelayedService.cpp

                ${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/tests-main.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/tests/Common/SchedulerKiller.cpp
        )

           target_compile_definitions(${PROJECT_NAME} PUBLIC UNIT_TESTS)

           target_include_directories(unittest_${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/tests )

           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC COMPILE_HOST)

           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC LINUX_PORT_DEBUG)

           target_compile_definitions(unittest_${PROJECT_NAME} PUBLIC MODULE_CORE_CUSTOM_BUS)

           target_compile_options(unittest_${PROJECT_NAME} PUBLIC -fsanitize=address )



           target_link_libraries(unittest_${PROJECT_NAME} PRIVATE pthread ${PROJECT_NAME})

           target_link_options(unittest_${PROJECT_NAME} PUBLIC -fsanitize=address)


endif()

