if (NOT IS_SYMLINK "${CMAKE_BINARY_DIR}/sys/user")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/sys/user")
endif()

add_catch2_executable(
    NAME
        db
    SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/unittest.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests-main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SMSTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThreadsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsNameTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsNumberTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsRingtonesTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsAddressTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactGroups_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SettingsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SettingsTable_v2_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/AlarmsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SMSTemplateTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/CalllogTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/NotificationsTable_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/EventsTable_tests.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/CalllogRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ContactsRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SMSRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ThreadRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SettingsRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/AlarmsRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SMSTemplateRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/NotificationsRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/EventsRecord_tests.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/QueryInterface.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/SettingsRecord_v2_tests.cpp"
        
    LIBS
        module-db
)
