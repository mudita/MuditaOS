project(service-db)
message("${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


set(SOURCES
    DBServiceAPI.cpp
    DBServiceAPI_GetByQuery.cpp
    DatabaseAgent.cpp
    ServiceDB.cpp
        messages/DBCalllogMessage.cpp
    messages/DBContactMessage.cpp
    messages/DBCountryCodeMessage.cpp
    messages/DBNotesMessage.cpp
    messages/DBNotificationMessage.cpp
    messages/DBServiceMessage.cpp
    messages/DBSMSMessage.cpp
    messages/DBSMSTemplateMessage.cpp
    messages/DBThreadMessage.cpp
    messages/QueryMessage.cpp
    agents/settings/SettingsAgent.cpp
    agents/settings/Settings.cpp
    agents/file_indexer/FileIndexerAgent.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-db
)

if (${ENABLE_TESTS})
    message(WARNING "Unit tests for service-db are disabled until they're fixed (EGD-5583).")
#    add_subdirectory(test)
endif ()

