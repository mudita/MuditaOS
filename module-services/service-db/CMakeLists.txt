include_directories( ${CMAKE_PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

message( "${PROJECT_NAME}  ${CMAKE_CURRENT_LIST_DIR}" )

target_sources( ${PROJECT_NAME}

    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ServiceDB.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/api/DBServiceAPI.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/messages/DBAlarmMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBCalllogMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBContactMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBCountryCodeMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBNotesMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBNotificationMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBSettingsMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBSMSMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBSMSTemplateMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/messages/DBThreadMessage.cpp"
)
