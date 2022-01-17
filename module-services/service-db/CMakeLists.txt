project(service-db)
message("${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )


set(SOURCES
    DBServiceAPI.cpp
    DBServiceAPI_GetByQuery.cpp
    DatabaseAgent.cpp
    ServiceDBCommon.cpp
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
    agents/quotes/QuotesAgent.cpp
    agents/quotes/RandomizedQuoteModel.cpp
    agents/quotes/QuotesSettingsSerializer.cpp
    )

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/agents>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        module-db
        json::json
    PUBLIC
        utf8
)

if (${ENABLE_TESTS})
    add_subdirectory(test)
endif ()
