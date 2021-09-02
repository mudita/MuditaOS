project(module-sys VERSION 1.0
        DESCRIPTION "Core module library")

module_is_test_entity()

set(BUS_IMPL_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/details/bus/Bus.cpp
)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/BusProxy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Worker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Message.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/Service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Timers/SystemTimer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Timers/TimerHandle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Timers/TimerFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Service/CpuSentinel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PhoneModes/Subject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/PhoneModes/Observer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/SystemManagerCommon.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/DependencyGraph.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/PowerManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/CpuStatistics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/DeviceManager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/CpuGovernor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemManager/graph/TopologicalSort.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SystemWatchdog/SystemWatchdog.cpp

        )

add_library(bus STATIC ${BUS_IMPL_SOURCES})
target_link_libraries(bus PRIVATE module-os)

target_compile_definitions(bus PRIVATE ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(bus PRIVATE ${PROJECT_TARGET})
target_include_directories(bus PRIVATE ${PROJECT_INCLUDES})

add_library(module-sys)
target_sources(module-sys PRIVATE ${SOURCES})

target_link_libraries(module-sys
    PRIVATE
        apps-common
        bus
        service-evtmgr
        messagetype
        magic_enum
        eventstore
    PUBLIC
        module-os
        module-bsp
    )

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ../module-services )

if (${ENABLE_TESTS})
    add_subdirectory(SystemManager/tests)
    add_subdirectory(Service/tests)
endif()
