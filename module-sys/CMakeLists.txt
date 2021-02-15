cmake_minimum_required(VERSION 3.14)

project(module-sys VERSION 1.0
        DESCRIPTION "Core module library")

set(BUS_IMPL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/details/bus/Bus.cpp
)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/BusProxy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Worker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Message.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Timer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/CpuSentinel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/SystemManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/DependencyGraph.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/PowerManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/CpuStatistics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/DeviceManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/CpuGovernor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/graph/TopologicalSort.cpp

        )

if(${PROJECT_TARGET} STREQUAL "TARGET_RT1051")
    include(targets/Target_RT1051.cmake)
elseif(${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Linux.cmake)
else()
    message(FATAL_ERROR "Invalid target!")
endif()

add_library(bus STATIC ${BUS_IMPL_SOURCES})
target_link_libraries(bus PRIVATE module-os)

target_compile_definitions(bus PRIVATE ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(bus PRIVATE ${PROJECT_TARGET})
target_compile_definitions(bus PRIVATE ${TARGET_COMPILE_DEFINITIONS})
target_compile_features(bus PRIVATE ${TARGET_COMPILE_FEATURES})
target_compile_options(bus PRIVATE ${TARGET_COMPILE_OPTIONS})
target_include_directories(bus PRIVATE ${BOARD_DIR_INCLUDES})
target_include_directories(bus PRIVATE ${PROJECT_INCLUDES})
target_link_options(bus PRIVATE ${TARGET_LINK_OPTIONS})

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        bus
        service-evtmgr
    PUBLIC
        module-os)

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ../module-services )
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})



target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (${ENABLE_TESTS})
    add_subdirectory(SystemManager/tests)
endif()
