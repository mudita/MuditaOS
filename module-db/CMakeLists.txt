cmake_minimum_required(VERSION 3.12)

project(module-db VERSION 1.0
        DESCRIPTION "Database module library")

if(NOT DEFINED PROJECT_LIB_DIRECTORY )
    set(PROJECT_LIB_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/" CACHE STRING "Output path for static libraries")
    message("Setting PROJECT_LIB_DIRECTORY to ${PROJECT_LIB_DIRECTORY}")
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_LIB_DIRECTORY})

if(NOT DEFINED PROJECT_BUILD_DIRECTORY )
    set(PROJECT_BUILD_DIRECTORY "${CMAKE_SOURCE_DIR}/build" CACHE STRING "Output directory for building" )
    message("Setting PROJECT_BUILD_DIRECTORY to ${PROJECT_BUILD_DIRECTORY}")
endif()

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BUILD_DIRECTORY})

set(SOURCES

        ${CMAKE_CURRENT_SOURCE_DIR}/Database/Field.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Database/QueryResult.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Database/Database.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Database/sqlite3.c
        ${CMAKE_CURRENT_SOURCE_DIR}/Database/sqlite3vfs.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Databases/ContactsDB.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Databases/SmsDB.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Databases/SettingsDB.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Databases/AlarmsDB.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Databases/NotesDB.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/Table.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/SMSTable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/ThreadsTable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/ContactsTable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/ContactsNameTable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/ContactsNumberTable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/ContactsRingtonesTable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/ContactsAddressTable.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Tables/SettingsTable.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Tables/AlarmsTable.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Tables/NotesTable.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Interface/Record.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Interface/SMSRecord.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Interface/ContactRecord.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Interface/ThreadRecord.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Interface/SettingsRecord.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Interface/AlarmsRecord.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Interface/NotesRecord.cpp

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

target_link_libraries(${PROJECT_NAME} module-utils module-vfs )

# Host target configuration(mainly used for unit testing)

if(DEFINED BUILD_UNIT_TESTS)

    add_subdirectory(tests)

endif()

