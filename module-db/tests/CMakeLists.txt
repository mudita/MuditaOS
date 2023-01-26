add_library(module-db-test_helpers Helpers.cpp)
add_library(module-db::test::helpers ALIAS module-db-test_helpers)
target_link_libraries(module-db-test_helpers PRIVATE module-db)
target_include_directories(module-db-test_helpers PUBLIC . )


add_catch2_executable(
    NAME
        db
    SRCS
        AlarmEventRecord_tests.cpp
        CalllogRecord_tests.cpp
        CalllogTable_tests.cpp
        ContactGroups_tests.cpp
        ContactsAddressTable_tests.cpp
        ContactsNameTable_tests.cpp
        ContactsNumberTable_tests.cpp
        ContactsRecord_tests.cpp
        ContactsRingtonesTable_tests.cpp
        ContactsTable_tests.cpp
        MultimediaFilesTable_tests.cpp
        NotesRecord_tests.cpp
        NotesTable_tests.cpp
        NotificationsTable_tests.cpp
        QueryInterface.cpp
        SMSRecord_tests.cpp
        SMSTable_tests.cpp
        SMSTemplateRecord_tests.cpp
        SMSTemplateTable_tests.cpp
        ThreadRecord_tests.cpp
        ThreadsTable_tests.cpp
        
    LIBS
        module-db::test::helpers
        module-db
        json::json
)
