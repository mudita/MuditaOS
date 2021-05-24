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
    agents/settings/SettingsProxy.cpp
    agents/settings/SettingsCache.cpp
    agents/settings/FactorySettings.cpp
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
        date
        module-apps
        module-db
        module-utils
        service-appmgr
        service-desktop
        service-fileindexer
)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()

