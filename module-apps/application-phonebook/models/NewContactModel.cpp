// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewContactModel.hpp"

#include "apps-common/windows/AppWindow.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"
#include "application-phonebook/widgets/InputBoxWithLabelAndIconWidget.hpp"
#include "application-phonebook/widgets/InputLinesWithLabelWidget.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"

#include <messages/DialogMetadataMessage.hpp>

NewContactModel::NewContactModel(app::ApplicationCommon *app) : application(app)
{}

auto NewContactModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto NewContactModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return phonebookStyle::inputLinesWithLabelWidget::h;
}

void NewContactModel::requestRecords(const std::uint32_t offset, const std::uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto NewContactModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void NewContactModel::createData()
{
    constexpr auto navBarOptionsLabelSide = gui::nav_bar::Side::Left;
    auto app                              = application;

    internalData.push_back(new gui::InputLinesWithLabelWidget(
        phonebookInternals::ListItemName::FirstName,
        [app](const UTF8 &text, bool emptyOthers) { app->getCurrentWindow()->navBarTemporaryMode(text, emptyOthers); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app](const UTF8 &text, bool active) {
            app->getCurrentWindow()->setNavBarText(text, navBarOptionsLabelSide);
            app->getCurrentWindow()->setNavBarActive(navBarOptionsLabelSide, active);
        },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        },
        [&](gui::Text *text) { openTextOptions(text); }));

    internalData.push_back(new gui::InputLinesWithLabelWidget(
        phonebookInternals::ListItemName::SecondName,
        [app](const UTF8 &text, bool emptyOthers) { app->getCurrentWindow()->navBarTemporaryMode(text, emptyOthers); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app](const UTF8 &text, bool active) {
            app->getCurrentWindow()->setNavBarText(text, navBarOptionsLabelSide);
            app->getCurrentWindow()->setNavBarActive(navBarOptionsLabelSide, active);
        },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        },
        [&](gui::Text *text) { openTextOptions(text); }));

    internalData.push_back(new gui::InputLinesWithLabelWidget(
        phonebookInternals::ListItemName::Number,
        [app](const UTF8 &text, bool emptyOthers) { app->getCurrentWindow()->navBarTemporaryMode(text, emptyOthers); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app](const UTF8 &text, bool active) {
            app->getCurrentWindow()->setNavBarText(text, navBarOptionsLabelSide);
            app->getCurrentWindow()->setNavBarActive(navBarOptionsLabelSide, active);
        },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        },
        [&](gui::Text *text) { openTextOptions(text); }));

    internalData.push_back(new gui::InputLinesWithLabelWidget(
        phonebookInternals::ListItemName::SecondNumber,
        [app](const UTF8 &text, bool emptyOthers) { app->getCurrentWindow()->navBarTemporaryMode(text, emptyOthers); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app](const UTF8 &text, bool active) {
            app->getCurrentWindow()->setNavBarText(text, navBarOptionsLabelSide);
            app->getCurrentWindow()->setNavBarActive(navBarOptionsLabelSide, active);
        },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        },
        [&](gui::Text *text) { openTextOptions(text); }));

    internalData.push_back(new gui::InputLinesWithLabelWidget(
        phonebookInternals::ListItemName::Email,
        [app](const UTF8 &text, bool emptyOthers) { app->getCurrentWindow()->navBarTemporaryMode(text, emptyOthers); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app](const UTF8 &text, bool active) {
            app->getCurrentWindow()->setNavBarText(text, navBarOptionsLabelSide);
            app->getCurrentWindow()->setNavBarActive(navBarOptionsLabelSide, active);
        },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        },
        [&](gui::Text *text) { openTextOptions(text); }));

    internalData.push_back(new gui::InputBoxWithLabelAndIconWidget(
        phonebookInternals::ListItemName::AddToFavourites,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); }));

    internalData.back()->setMargins(gui::Margins(style::widgets::leftMargin, style::margins::very_big, 0, 0));

    internalData.push_back(new gui::InputBoxWithLabelAndIconWidget(
        phonebookInternals::ListItemName::AddToICE,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); }));

    internalData.back()->setMargins(
        gui::Margins(style::widgets::leftMargin, style::margins::big, 0, style::margins::very_small));

    internalData.push_back(new gui::InputLinesWithLabelWidget(
        phonebookInternals::ListItemName::Address,
        [app](const UTF8 &text, bool emptyOthers) { app->getCurrentWindow()->navBarTemporaryMode(text, emptyOthers); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app](const UTF8 &text, bool active) {
            app->getCurrentWindow()->setNavBarText(text, navBarOptionsLabelSide);
            app->getCurrentWindow()->setNavBarActive(navBarOptionsLabelSide, active);
        },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        },
        [&](gui::Text *text) { openTextOptions(text); },
        2));

    internalData.push_back(new gui::InputLinesWithLabelWidget(
        phonebookInternals::ListItemName::Note,
        [app](const UTF8 &text, bool emptyOthers) { app->getCurrentWindow()->navBarTemporaryMode(text, emptyOthers); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app](const UTF8 &text, bool active) {
            app->getCurrentWindow()->setNavBarText(text, navBarOptionsLabelSide);
            app->getCurrentWindow()->setNavBarActive(navBarOptionsLabelSide, active);
        },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        },
        [&](gui::Text *text) { openTextOptions(text); },
        2));

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void NewContactModel::clearData()
{
    list->reset();
    eraseInternalData();
}

bool NewContactModel::verifyData()
{
    for (const auto &item : internalData) {
        if (item->onVerifyCallback) {
            std::string errorMessage;
            if (!item->onVerifyCallback(errorMessage)) {
                auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                    gui::DialogMetadata{errorMessage,
                                        "fail_128px_W_G",
                                        utils::translate("app_phonebook_new_contact_invalid_number"),
                                        "",
                                        [=]() -> bool {
                                            application->returnToPreviousWindow();
                                            return true;
                                        }});

                application->switchWindow(gui::window::name::dialog, std::move(metaData));

                return false;
            }
        }
    }
    return true;
}

void NewContactModel::saveData(std::shared_ptr<ContactRecord> contactRecord)
{
    for (const auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(contactRecord);
        }
    }
}

void NewContactModel::loadData(std::shared_ptr<ContactRecord> contactRecord, gui::SwitchData *data)
{
    for (const auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(contactRecord);
        }
    }

    if (const auto message = dynamic_cast<PhonebookItemData *>(data); message != nullptr) {
        requestType = message->getRequestType();
    }
}

bool NewContactModel::emptyData()
{
    for (const auto &item : internalData) {
        if (item->onEmptyCallback) {
            if (!item->onEmptyCallback()) {
                return false;
            }
        }
    }
    return true;
}

bool NewContactModel::readyToSave()
{
    // The contact model is ready to be saved when any of the phone number fields are not empty
    for (const auto &item : internalData) {
        if (auto itemWidget = dynamic_cast<gui::InputLinesWithLabelWidget *>(item); itemWidget != nullptr) {
            if (auto typeOfItem = itemWidget->getListItemName();
                (typeOfItem == phonebookInternals::ListItemName::Number ||
                 typeOfItem == phonebookInternals::ListItemName::SecondNumber) &&
                itemWidget->onEmptyCallback && !itemWidget->onEmptyCallback()) {
                return false;
            }
        }
    }
    return true;
}
[[nodiscard]] auto NewContactModel::getRequestType() -> PhonebookItemData::RequestType
{
    return requestType;
}

void NewContactModel::openTextOptions(gui::Text *text)
{
    auto data = std::make_unique<PhonebookInputOptionData>(text);
    application->switchWindow(gui::window::name::input_options, std::move(data));
}
bool NewContactModel::isAnyUnsavedChange(std::shared_ptr<ContactRecord> contactRecord)
{
    for (const auto &item : internalData) {
        if (item->onCheckUnsavedChangeCallback) {
            if (item->onCheckUnsavedChangeCallback(contactRecord)) {
                return true;
            }
        }
    }
    return false; // there is no change between already provided data and saved ones
}
