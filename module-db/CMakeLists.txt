cmake_minimum_required(VERSION 3.12)

project(module-db VERSION 1.0
        DESCRIPTION "Database module library")

set(SOURCES

        Database/Field.cpp
        Database/QueryResult.cpp
        Database/Database.cpp
        Database/sqlite3.c
        Database/sqlite3vfs.cpp

        Databases/ContactsDB.cpp
        Databases/SmsDB.cpp
        Databases/SettingsDB.cpp
        Databases/AlarmsDB.cpp
        Databases/NotesDB.cpp
        Databases/CalllogDB.cpp
        Databases/CountryCodesDB.cpp

        Tables/Table.cpp
        Tables/SMSTable.cpp
        Tables/ThreadsTable.cpp
        Tables/ContactsTable.cpp
        Tables/ContactsNameTable.cpp
        Tables/ContactsNumberTable.cpp
        Tables/ContactsRingtonesTable.cpp
        Tables/ContactsAddressTable.cpp
        Tables/SettingsTable.cpp
		Tables/AlarmsTable.cpp
		Tables/NotesTable.cpp
        Tables/CalllogTable.cpp
        Tables/CountryCodesTable.cpp
        Tables/SMSTemplateTable.cpp

        Interface/Record.cpp
        Interface/SMSRecord.cpp
        Interface/ContactRecord.cpp
        Interface/ThreadRecord.cpp
        Interface/SettingsRecord.cpp
        Interface/AlarmsRecord.cpp
        Interface/NotesRecord.cpp
        Interface/CalllogRecord.cpp
        Interface/SMSTemplateRecord.cpp

        queries/sms/QuerySMSSearch.cpp
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
add_subdirectory(tests)

