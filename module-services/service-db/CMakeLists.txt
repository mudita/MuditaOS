message("${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}")

target_sources( ${PROJECT_NAME}

    PRIVATE
        "ServiceDB.cpp"
        "api/DBServiceAPI.cpp"
        "api/DBServiceAPI_GetByQuery.cpp"

        "messages/DBAlarmMessage.cpp"
        "messages/DBCalllogMessage.cpp"
        "messages/DBContactMessage.cpp"
        "messages/DBCountryCodeMessage.cpp"
        "messages/DBNotesMessage.cpp"
        "messages/DBNotificationMessage.cpp"
        "messages/DBSettingsMessage.cpp"
        "messages/DBSMSMessage.cpp"
        "messages/DBSMSTemplateMessage.cpp"
        "messages/DBThreadMessage.cpp"
        "messages/QueryMessage.cpp"
)
