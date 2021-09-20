project(service-time)
message( "${PROJECT_NAME}  ${CMAKE_CURRENT_SOURCE_DIR}" )

set(SOURCES
    AlarmHandlerFactory.cpp
    AlarmEventsDBRepository.cpp
    AlarmMessageHandler.cpp
    AlarmOperations.cpp
    AlarmServiceAPI.cpp
    ServiceTime.cpp
    TimeManager.cpp
    RTCcommand.cpp
    TimezoneHandler.cpp
    internal/StaticData.cpp

    include/service-time/AlarmHandler.hpp
    include/service-time/AlarmHandlerFactory.hpp
    include/service-time/AlarmMessage.hpp
    include/service-time/AlarmServiceAPI.hpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
        include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-db
        time
        service-db
        service-time-api
        module-sys
        module-apps
)

add_subdirectory(api)

if (NOT (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang"))
else()
target_compile_options(${PROJECT_NAME}
    PRIVATE
    -Wno-potentially-evaluated-expression
)
endif()

if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()
