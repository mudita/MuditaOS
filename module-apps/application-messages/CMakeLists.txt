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
        "ApplicationMessages.cpp"
        "widgets/ThreadItem.cpp"
        "widgets/SMSTemplateModel.cpp"
        "widgets/SMSTemplateItem.cpp"
        "widgets/Icon.cpp"
        "widgets/SearchResultsItem.cpp"
        "widgets/BaseThreadItem.cpp"

        "models/BaseThreadRecordModel.cpp"
        "models/ThreadModel.cpp"
        "models/SearchResultsModel.cpp"
        
        "windows/MessagesMainWindow.cpp"
        "windows/ThreadViewWindow.cpp"
        "windows/NewMessage.cpp"
        "windows/OptionsWindow.cpp"
        "windows/OptionsMessages.cpp"
        "windows/SearchStart.cpp"
        "windows/SMSTemplatesWindow.cpp"
        "windows/SearchResults.cpp"

        "data/SMSTextToSearch.cpp"

    PUBLIC
        "ApplicationMessages.hpp"
        "MessagesStyle.hpp"
        "models/ThreadModel.hpp"
        "widgets/ThreadItem.hpp"
)

