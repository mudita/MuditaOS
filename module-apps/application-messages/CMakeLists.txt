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
        "models/SMSTemplateModel.cpp"
        "widgets/SMSTemplateItem.cpp"
        "widgets/SMSInputWidget.cpp"
        "widgets/SMSOutputWidget.cpp"
        "widgets/SearchResultsItem.cpp"
        "widgets/BaseThreadItem.cpp"

        "models/BaseThreadsRecordModel.cpp"
        "models/ThreadsModel.cpp"
        "models/ThreadsSearchResultsModel.cpp"
        "models/SMSThreadModel.cpp"
        
        "windows/MessagesMainWindow.cpp"
        "windows/SMSThreadViewWindow.cpp"
        "windows/NewMessage.cpp"
        "windows/ThreadWindowOptions.cpp"
        "windows/OptionsMessages.cpp"
        "windows/SearchStart.cpp"
        "windows/SMSTemplatesWindow.cpp"
        "windows/SearchResults.cpp"

        "data/SMSTextToSearch.cpp"

    PUBLIC
        "ApplicationMessages.hpp"
        "Constants.hpp"
        "data/MessagesStyle.hpp"
        "models/ThreadsModel.hpp"
        "models/SMSThreadModel.hpp"
        "widgets/ThreadItem.hpp"
        "widgets/SMSInputWidget.hpp"
        "widgets/SMSOutputWidget.hpp"
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        service-db
        clipboard
    )
