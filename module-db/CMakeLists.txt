project(module-db VERSION 1.0
        DESCRIPTION "Database module library")

module_is_test_entity()

include(thirdparty)

set (SQLITE3_SOURCE Database/sqlite3.c)

set(SOURCES
        Common/Query.cpp

        Database/Field.cpp
        Database/QueryResult.cpp
        Database/Database.cpp
        Database/DatabaseInitializer.cpp
        Database/sqlite3vfs.cpp
        ${SQLITE3_SOURCE}

        Databases/CalllogDB.cpp
        Databases/ContactsDB.cpp
        Databases/CountryCodesDB.cpp
        Databases/EventsDB.cpp
        Databases/MultimediaFilesDB.cpp
        Databases/NotesDB.cpp
        Databases/NotificationsDB.cpp
        Databases/SmsDB.cpp

        Tables/AlarmEventsTable.cpp
        Tables/Table.cpp
        Tables/SMSTable.cpp
        Tables/ThreadsTable.cpp
        Tables/ContactsTable.cpp
        Tables/ContactsNameTable.cpp
        Tables/ContactsNumberTable.cpp
        Tables/ContactsRingtonesTable.cpp
        Tables/ContactsAddressTable.cpp
        Tables/ContactsGroups.cpp
        Tables/NotesTable.cpp
        Tables/CalllogTable.cpp
        Tables/CountryCodesTable.cpp
        Tables/SMSTemplateTable.cpp
        Tables/NotificationsTable.cpp
        Tables/MultimediaFilesTable.cpp

        Interface/AlarmEventRecord.cpp
        Interface/CalllogRecord.cpp
        Interface/ContactRecord.cpp
        Interface/EventRecord.cpp
        Interface/MultimediaFilesRecord.cpp
        Interface/NotesRecord.cpp
        Interface/NotificationsRecord.cpp
        Interface/Record.cpp
        Interface/SMSRecord.cpp
        Interface/SMSTemplateRecord.cpp
        Interface/ThreadRecord.cpp

        queries/RecordQuery.cpp
        queries/alarm_events/QueryAlarmEventsAdd.cpp
        queries/alarm_events/QueryAlarmEventsEdit.cpp
        queries/alarm_events/QueryAlarmEventsGet.cpp
        queries/alarm_events/QueryAlarmEventsGetBetweenDates.cpp
        queries/alarm_events/QueryAlarmEventsGetLimited.cpp
        queries/alarm_events/QueryAlarmEventsGetNext.cpp
        queries/alarm_events/QueryAlarmEventsGetRecurringBetweenDates.cpp
        queries/alarm_events/QueryAlarmEventsRemove.cpp
        queries/calllog/QueryCalllogGet.cpp
        queries/calllog/QueryCalllogGetByContactID.cpp
        queries/calllog/QueryCalllogGetCount.cpp
        queries/calllog/QueryCalllogRemove.cpp
        queries/calllog/QueryCalllogSetAllRead.cpp
        queries/messages/sms/QuerySMSAdd.cpp
        queries/messages/sms/QuerySMSGet.cpp
        queries/messages/sms/QuerySMSGetByContactID.cpp
        queries/messages/sms/QuerySMSGetByID.cpp
        queries/messages/sms/QuerySMSGetByText.cpp
        queries/messages/sms/QuerySMSGetByThreadID.cpp
        queries/messages/sms/QuerySMSGetCount.cpp
        queries/messages/sms/QuerySMSGetCountByThreadID.cpp
        queries/messages/sms/QuerySMSGetForList.cpp
        queries/messages/sms/QuerySMSGetLastByThreadID.cpp
        queries/messages/sms/QuerySMSRemove.cpp
        queries/messages/sms/QuerySMSSearchByType.cpp
        queries/messages/sms/QuerySMSUpdate.cpp
        queries/messages/templates/QuerySMSTemplateAdd.cpp
        queries/messages/templates/QuerySMSTemplateGet.cpp
        queries/messages/templates/QuerySMSTemplateGetByID.cpp
        queries/messages/templates/QuerySMSTemplateGetCount.cpp
        queries/messages/templates/QuerySMSTemplateGetForList.cpp
        queries/messages/templates/QuerySMSTemplateRemove.cpp
        queries/messages/templates/QuerySMSTemplateUpdate.cpp
        queries/messages/threads/QueryThreadGetByContactID.cpp
        queries/messages/threads/QueryThreadGetByID.cpp
        queries/messages/threads/QueryThreadGetByNumber.cpp
        queries/messages/threads/QueryThreadMarkAsRead.cpp
        queries/messages/threads/QueryThreadRemove.cpp
        queries/messages/threads/QueryThreadsGet.cpp
        queries/messages/threads/QueryThreadsGetCount.cpp
        queries/messages/threads/QueryThreadsGetForList.cpp
        queries/messages/threads/QueryThreadsSearchForList.cpp
        queries/multimedia_files/QueryMultimediaFilesAdd.cpp
        queries/multimedia_files/QueryMultimediaFilesEdit.cpp
        queries/multimedia_files/QueryMultimediaFilesGet.cpp
        queries/multimedia_files/QueryMultimediaFilesGetLimited.cpp
        queries/multimedia_files/QueryMultimediaFilesRemove.cpp
        queries/multimedia_files/QueryMultimediaFilesCount.cpp
        queries/notes/QueryNoteRemove.cpp
        queries/notes/QueryNotesGet.cpp
        queries/notes/QueryNotesGetByText.cpp
        queries/notes/QueryNoteStore.cpp
        queries/notifications/QueryNotificationsClear.cpp
        queries/notifications/QueryNotificationsGet.cpp
        queries/notifications/QueryNotificationsGetAll.cpp
        queries/notifications/QueryNotificationsIncrement.cpp
        queries/notifications/QueryNotificationsMultipleIncrement.cpp
        queries/phonebook/QueryCheckContactsListDuplicates.cpp
        queries/phonebook/QueryContactAdd.cpp
        queries/phonebook/QueryContactGet.cpp
        queries/phonebook/QueryContactGetByID.cpp
        queries/phonebook/QueryContactRemove.cpp
        queries/phonebook/QueryContactUpdate.cpp
        queries/phonebook/QueryMergeContactsList.cpp
        queries/phonebook/QueryNumberGetByID.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Board specific compilation definitions,options,include directories and features
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_CONFIG_DEFINITIONS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${PROJECT_TARGET})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_INCLUDES})

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Database/sqlite3vfs.cpp PROPERTIES COMPILE_FLAGS -Wno-overflow)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Database/sqlite3.c PROPERTIES COMPILE_FLAGS "-DSQLITE_DEFAULT_WAL_AUTOCHECKPOINT=1 -Wno-misleading-indentation")

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

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        i18n
        module-utils
        module-vfs
        utf8
    PRIVATE
        application-calendar
        Microsoft.GSL::GSL
        rrule
        platform
)

# Host target configuration(mainly used for unit testing)
if (${ENABLE_TESTS})
    add_subdirectory(tests)
endif ()

third_party_source_optimization(${SQLITE3_SOURCE})
