project("unittest_module-sys" VERSION 1.0 DESCRIPTION "module-sys tests")

add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL
    Test1/TestService1.cpp
    Test1/CommandsService.cpp
    Test1/DelayedService.cpp

    tests.cpp
    tests-main.cpp
    Common/SchedulerKiller.cpp
    )

#target_compile_definitions(${PROJECT_NAME} PUBLIC UNIT_TESTS)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(${PROJECT_NAME} PUBLIC COMPILE_HOST)
target_compile_definitions(${PROJECT_NAME} PUBLIC LINUX_PORT_DEBUG)
target_compile_definitions(${PROJECT_NAME} PUBLIC MODULE_CORE_CUSTOM_BUS)

target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address )
target_link_libraries(${PROJECT_NAME} PRIVATE pthread ${PROJECT_NAME})
target_link_options(${PROJECT_NAME} PUBLIC -fsanitize=address)

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME}
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         )
add_dependencies(check ${PROJECT_NAME})
