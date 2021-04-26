project(service-db)
message("${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


set(SOURCES
    DBServiceAPI.cpp
    DBServiceAPI_GetByQuery.cpp
    DatabaseAgent.cpp
    ServiceDB.cpp
    EntryPath.cpp
    messages/DBCalllogMessage.cpp
    messages/DBContactMessage.cpp
    messages/DBNotificationMessage.cpp
    messages/DBServiceMessage.cpp
    messages/QueryMessage.cpp
    agents/settings/SettingsAgent.cpp
    agents/settings/Settings.cpp
    agents/settings/SettingsInterface.cpp
    agents/settings/SettingsCache.cpp
    agents/settings/FactorySettings.cpp
    agents/settings/SettingsMessages.cpp
    agents/file_indexer/FileIndexerAgent.cpp
    agents/quotes/QuotesAgent.cpp
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
    add_subdirectory(test)
endif ()

