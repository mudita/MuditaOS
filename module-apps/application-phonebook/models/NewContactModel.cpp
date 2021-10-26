// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewContactModel.hpp"

#include "AppWindow.hpp"
#include "application-phonebook/widgets/ContactListItem.hpp"
#include "application-phonebook/widgets/InputBoxWithLabelAndIconWidget.hpp"
#include "application-phonebook/widgets/InputLinesWithLabelIWidget.hpp"

#include <ListView.hpp>
#include <time/ScopedTime.hpp>
#include <NavBar.hpp>

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

void NewContactModel::requestRecords(const uint32_t offset, const uint32_t limit)
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
    auto app = application;

    internalData.push_back(new gui::InputLinesWithLabelIWidget(
        phonebookInternals::ListItemName::FirstName,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->ContactDataChanged(); }));

    internalData.push_back(new gui::InputLinesWithLabelIWidget(
        phonebookInternals::ListItemName::SecondName,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->ContactDataChanged(); }));

    internalData.push_back(new gui::InputLinesWithLabelIWidget(
        phonebookInternals::ListItemName::Number,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->ContactDataChanged(); }));

    internalData.push_back(new gui::InputLinesWithLabelIWidget(
        phonebookInternals::ListItemName::SecondNumber,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->ContactDataChanged(); }));

    internalData.push_back(new gui::InputLinesWithLabelIWidget(
        phonebookInternals::ListItemName::Email,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->ContactDataChanged(); }));

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

    internalData.push_back(new gui::InputLinesWithLabelIWidget(
        phonebookInternals::ListItemName::Address,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        nullptr,
        2));

    internalData.push_back(new gui::InputLinesWithLabelIWidget(
        phonebookInternals::ListItemName::Note,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        nullptr,
        2));

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void NewContactModel::clearData()
{
    list->reset();

    eraseInternalData();

    createData();

    list->rebuildList();
}

void NewContactModel::saveData(std::shared_ptr<ContactRecord> contactRecord)
{
    for (auto item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(contactRecord);
        }
    }
}

void NewContactModel::loadData(std::shared_ptr<ContactRecord> contactRecord)
{
    for (auto item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(contactRecord);
        }
    }
}

void NewContactModel::ContactDataChanged()
{
    for (auto item : internalData) {
        if (item->onEmptyCallback) {
            if (!item->onEmptyCallback()) {
                application->getCurrentWindow()->setNavBarActive(gui::nav_bar::Side::Center, true); // SAVE button
                return;
            }
        }
    }
    application->getCurrentWindow()->setNavBarActive(gui::nav_bar::Side::Center, false); // SAVE button
    return;
}
