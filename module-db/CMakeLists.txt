cmake_minimum_required(VERSION 3.12)

project(module-db VERSION 1.0
        DESCRIPTION "Database module library")

include(thirdparty)

set (SQLITE3_SOURCE Database/sqlite3.c)

set(SOURCES
        Common/Query.cpp

        Database/Field.cpp
        Database/QueryResult.cpp
        Database/Database.cpp
        Database/sqlite3vfs.cpp
        ${SQLITE3_SOURCE}

        Databases/ContactsDB.cpp
        Databases/SmsDB.cpp
        Databases/SettingsDB.cpp
        Databases/AlarmsDB.cpp
        Databases/NotesDB.cpp
        Databases/CalllogDB.cpp
        Databases/CountryCodesDB.cpp
        Databases/NotificationsDB.cpp
        Databases/EventsDB.cpp

        Tables/Table.cpp
        Tables/SMSTable.cpp
        Tables/ThreadsTable.cpp
        Tables/ContactsTable.cpp
        Tables/ContactsNameTable.cpp
        Tables/ContactsNumberTable.cpp
        Tables/ContactsRingtonesTable.cpp
        Tables/ContactsAddressTable.cpp
        Tables/ContactsGroups.cpp
        Tables/SettingsTable.cpp
        Tables/AlarmsTable.cpp
        Tables/NotesTable.cpp
        Tables/CalllogTable.cpp
        Tables/CountryCodesTable.cpp
        Tables/SMSTemplateTable.cpp
        Tables/NotificationsTable.cpp
        Tables/EventsTable.cpp
        Tables/SettingsTable_v2.cpp

        Interface/Record.cpp
        Interface/SMSRecord.cpp
        Interface/ContactRecord.cpp
        Interface/ThreadRecord.cpp
        Interface/SettingsRecord.cpp
        Interface/AlarmsRecord.cpp
        Interface/NotesRecord.cpp
        Interface/CalllogRecord.cpp
        Interface/SMSTemplateRecord.cpp
        Interface/NotificationsRecord.cpp
        Interface/EventsRecord.cpp
        Interface/SettingsRecord_v2.cpp

        queries/RecordQuery.cpp
        queries/sms/QuerySMSSearch.cpp
        queries/sms/QuerySMSThreadsGet.cpp
        queries/sms/QuerySMSSearchByType.cpp
        queries/sms/QuerySmsThreadMarkAsRead.cpp
        queries/sms/QuerySMSGet.cpp
        queries/sms/QuerySMSGetCount.cpp
        queries/sms/QuerySMSGetByID.cpp
        queries/sms/QuerySMSGetByContactID.cpp
        queries/sms/QuerySMSGetByThreadID.cpp
        queries/sms/QuerySMSGetByText.cpp
        queries/sms/QuerySMSRemove.cpp
        queries/sms/QuerySMSTemplateAdd.cpp
        queries/sms/QuerySMSTemplateGetByID.cpp
        queries/sms/QuerySMSTemplateGet.cpp
        queries/sms/QuerySMSTemplateGetCount.cpp
        queries/sms/QuerySMSTemplateRemove.cpp
        queries/sms/QuerySMSTemplateUpdate.cpp
        queries/sms/QueryThreadGetByNumber.cpp
        queries/calllog/QueryCalllogSetAllRead.cpp
        queries/calllog/QueryCalllogGet.cpp
        queries/calllog/QueryCalllogGetCount.cpp
        queries/calllog/QueryCalllogRemove.cpp
        queries/calllog/QueryCalllogGetByContactID.cpp
        queries/notifications/QueryNotificationsGet.cpp
        queries/notifications/QueryNotificationsIncrement.cpp
        queries/notifications/QueryNotificationsClear.cpp
        queries/notifications/QueryNotificationsGetAll.cpp
        queries/phonebook/QueryContactGet.cpp
        queries/phonebook/QueryContactGetByID.cpp
        queries/phonebook/QueryContactAdd.cpp
        queries/phonebook/QueryContactUpdate.cpp
        queries/phonebook/QueryContactRemove.cpp
        queries/phonebook/QueryNumberGetByID.cpp
        queries/calendar/QueryEventsRemove.cpp
        queries/calendar/QueryEventsGet.cpp
        queries/calendar/QueryEventsGetAll.cpp
        queries/calendar/QueryEventsAdd.cpp
        queries/calendar/QueryEventsEdit.cpp
        queries/calendar/QueryEventsGetFiltered.cpp
        queries/calendar/QueryEventsGetAllLimited.cpp
        queries/settings/QuerySettingsGet_v2.cpp
        queries/settings/QuerySettingsUpdate_v2.cpp
        queries/settings/QuerySettingsAddOrIgnore_v2.cpp
)

if(NOT ${PROJECT_TARGET} STREQUAL "TARGET_Linux")
    include(targets/Target_Cross.cmake)
else()
    include(targets/Target_Linux.cmake)
endif()

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${BOARD_SOURCES})

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_DEFINITIONS})
target_include_directories(${PROJECT_NAME} PUBLIC ${BOARD_DIR_INCLUDES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})
target_compile_features(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_FEATURES})
target_compile_options(${PROJECT_NAME} PUBLIC ${TARGET_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PUBLIC ${TARGET_LINK_OPTIONS})


set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Database/sqlite3vfs.cpp PROPERTIES COMPILE_FLAGS -Wno-overflow)

target_compile_definitions(${PROJECT_NAME}

        PUBLIC

        -D_HAVE_SQLITE_CONFIG_H
)

target_include_directories(${PROJECT_NAME}

        PUBLIC

        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Interface

        PRIVATE

        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Interface
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables
        ${CMAKE_CURRENT_SOURCE_DIR}/Databases
        ${CMAKE_CURRENT_SOURCE_DIR}/Database
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    -Wno-error=return-local-addr
)

target_link_libraries(${PROJECT_NAME} module-utils module-vfs )

# Host target configuration(mainly used for unit testing)
if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()

third_party_source_optimization(${SQLITE3_SOURCE})
