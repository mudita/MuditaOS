add_catch2_executable(
        NAME
        timezone-handler
        SRCS
        unittest_timezone_handler.cpp
        LIBS
        service-time
        module-sys
)

add_catch2_executable(
    NAME
        service-time-alarm-tests
    SRCS
        tests-AlarmHandlerFactory.cpp
        MockAlarmHandler.hpp
    LIBS
        service-time
        module-db
)

add_library(service-time-alarm-operations-source OBJECT
    tests-AlarmOperations.cpp)

target_link_libraries(service-time-alarm-operations-source
    PUBLIC
        module-sys
        service-time
        gtest_main
        gmock
        log-api)

target_include_directories(service-time-alarm-operations-source
    PUBLIC
        $<TARGET_PROPERTY:service-time,INCLUDE_DIRECTORIES>)

add_executable(googletest-service-time-alarm-operations)
target_link_libraries(googletest-service-time-alarm-operations
    PUBLIC
    service-time-alarm-operations-source
)

add_gtest_manually_prepared_target(NAME googletest-service-time-alarm-operations)
