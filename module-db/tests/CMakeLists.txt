cmake_minimum_required(VERSION 3.12)

project(unittest_module-db VERSION 1.0
        DESCRIPTION "Database module library unit tests")

add_executable( ${PROJECT_NAME} EXCLUDE_FROM_ALL
        "${CMAKE_CURRENT_SOURCE_DIR}/unittest.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests-main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SMSTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThreadsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsNameTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsNumberTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsRingtonesTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsAddressTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SettingsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/AlarmsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SMSTemplateTable_tests.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SMSRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThreadRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SettingsRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/AlarmsRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SMSTemplateRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/QueryInterface.cpp"
        )

target_link_libraries( ${PROJECT_NAME} PRIVATE module-db )

add_test(NAME ${PROJECT_NAME}
         COMMAND ${PROJECT_NAME}
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
         )
add_dependencies(check ${PROJECT_NAME})
