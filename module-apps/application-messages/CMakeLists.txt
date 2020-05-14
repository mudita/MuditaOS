include_directories( ${CMAKE_PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

include_directories( ${PROJECT_NAME}

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}"
)

target_sources( ${PROJECT_NAME}

    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationMessages.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/ThreadModel.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/ThreadItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/SMSTemplateModel.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/SMSTemplateItem.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/Icon.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/SearchResults.cpp"

        "${CMAKE_CURRENT_LIST_DIR}/models/SearchResultsModel.cpp"
        
        "${CMAKE_CURRENT_LIST_DIR}/windows/MessagesMainWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/ThreadViewWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/NewMessage.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/OptionsWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/OptionsMessages.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/SearchStart.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/SMSTemplatesWindow.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/windows/SearchResults.cpp"

    PUBLIC
        "${CMAKE_CURRENT_LIST_DIR}/ApplicationMessages.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/MessagesStyle.hpp"
         "${CMAKE_CURRENT_LIST_DIR}/widgets/ThreadModel.hpp"
        "${CMAKE_CURRENT_LIST_DIR}/widgets/ThreadItem.hpp"

)

