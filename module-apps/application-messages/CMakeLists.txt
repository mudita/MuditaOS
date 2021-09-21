# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-messages STATIC)

target_include_directories(application-messages
    PRIVATE
        $<BUILD_INTERFACE:
            ${CMAKE_CURRENT_SOURCE_DIR}/data
            ${CMAKE_CURRENT_SOURCE_DIR}/include/application-messages
            ${CMAKE_CURRENT_SOURCE_DIR}/models
            ${CMAKE_CURRENT_SOURCE_DIR}/widgets
            ${CMAKE_CURRENT_SOURCE_DIR}/windows
        >
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(application-messages
    PRIVATE
        ApplicationMessages.cpp
        data/MessagesStyle.hpp
        data/SMSdata.hpp
        data/SMSTextToSearch.cpp
        data/SMSTextToSearch.hpp
        models/BaseThreadsRecordModel.cpp
        models/BaseThreadsRecordModel.hpp
        models/SMSTemplateModel.cpp
        models/SMSTemplateModel.hpp
        models/SMSThreadModel.cpp
        models/SMSThreadModel.hpp
        models/ThreadsModel.cpp
        models/ThreadsModel.hpp
        models/ThreadsSearchResultsModel.cpp
        models/ThreadsSearchResultsModel.hpp
        widgets/BaseThreadItem.cpp
        widgets/BaseThreadItem.hpp
        widgets/SearchResultsItem.cpp
        widgets/SearchResultsItem.hpp
        widgets/SMSInputWidget.cpp
        widgets/SMSInputWidget.hpp
        widgets/SMSOutputWidget.cpp
        widgets/SMSOutputWidget.hpp
        widgets/SMSTemplateItem.cpp
        widgets/SMSTemplateItem.hpp
        widgets/ThreadItem.cpp
        widgets/ThreadItem.hpp
        windows/MessagesMainWindow.cpp
        windows/MessagesMainWindow.hpp
        windows/NewMessage.cpp
        windows/NewMessage.hpp
        windows/OptionsMessages.cpp
        windows/OptionsMessages.hpp
        windows/SearchResults.cpp
        windows/SearchResults.hpp
        windows/SearchStart.cpp
        windows/SearchStart.hpp
        windows/SMSTemplatesWindow.cpp
        windows/SMSTemplatesWindow.hpp
        windows/SMSThreadViewWindow.cpp
        windows/SMSThreadViewWindow.hpp
        windows/ThreadWindowOptions.cpp
        windows/ThreadWindowOptions.hpp
    PUBLIC
        include/application-messages/ApplicationMessages.hpp
        include/application-messages/Constants.hpp
)

target_link_libraries(application-messages
    PRIVATE
        app
        clipboard
        i18n
        log
        module-gui
        service-appmgr
        service-cellular
        service-db
        utils-time
    PUBLIC
        apps-common
        module-db
        module-utils
)

option(MESSAGE_TEXT_SEARCH "Enable message text search functionality" OFF)
if(${MESSAGE_TEXT_SEARCH})
    target_compile_definitions(application-messages PRIVATE MESSAGE_TEXT_SEARCH=1)
endif()
