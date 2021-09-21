# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

add_library(application-phonebook STATIC)

target_include_directories(application-phonebook
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


target_sources(application-phonebook
    PRIVATE
        ApplicationPhonebook.cpp
        data/PhonebookUtils.cpp
        models/ContactDetailsModel.cpp
        models/NewContactModel.cpp
        models/PhonebookModel.cpp
        widgets/ContactFlagIconWidget.cpp
        widgets/ContactFlagsWidget.cpp
        widgets/InformationWidget.cpp
        widgets/InputBoxWithLabelAndIconWidget.cpp
        widgets/InputLinesWithLabelIWidget.cpp
        widgets/OutputLinesTextWithLabelWidget.cpp
        widgets/PhonebookItem.cpp
        widgets/PhonebookListView.cpp
        windows/PhonebookContactDetails.cpp
        windows/PhonebookContactOptions.cpp
        windows/PhonebookIceContacts.cpp
        windows/PhonebookMainWindow.cpp
        windows/PhonebookNamecardOptions.cpp
        windows/PhonebookNewContact.cpp
        windows/PhonebookSearch.cpp
        windows/PhonebookSearchResults.cpp
    PRIVATE
        data/ContactsMap.hpp
        models/ContactDetailsModel.hpp
        models/NewContactModel.hpp
        models/PhonebookModel.hpp
        widgets/ContactListItem.hpp
        widgets/InformationWidget.hpp
        widgets/InputBoxWithLabelAndIconWidget.hpp
        widgets/InputLinesWithLabelIWidget.hpp
        widgets/OutputLinesTextWithLabelWidget.hpp
        widgets/PhonebookItem.hpp
        widgets/PhonebookListView.hpp
        windows/PhonebookContactDetails.hpp
        windows/PhonebookIceContacts.hpp
        windows/PhonebookNamecardOptions.hpp
        windows/PhonebookNewContact.hpp
    PUBLIC
        include/application-phonebook/ApplicationPhonebook.hpp
)

option(ENABLE_APP_PHONEBOOK "Enable application phonebook" ON)

target_compile_definitions(application-phonebook
    INTERFACE
        $<$<BOOL:${ENABLE_APP_PHONEBOOK}>:ENABLE_APP_PHONEBOOK>
)

target_link_libraries(application-phonebook
    PRIVATE
        app
        utf8
        module-gui
        module-utils
        utils-time
        module-db
        service-appmgr
        service-db
        i18n
    PUBLIC
        apps-common
)
