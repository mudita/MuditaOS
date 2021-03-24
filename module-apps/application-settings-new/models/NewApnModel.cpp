// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewApnModel.hpp"

#include "application-settings-new/widgets/ApnInputWidget.hpp"

#include <ListView.hpp>
#include <time/ScopedTime.hpp>
#include <BottomBar.hpp>

NewApnModel::NewApnModel(app::Application *app) : application(app)
{}

auto NewApnModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto NewApnModel::getMinimalItemHeight() const -> unsigned int
{
    return style::settings::widget::apnInputWidget::h;
}

void NewApnModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto NewApnModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void NewApnModel::createData()
{
    auto app = application;

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::Name,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->apnDataChanged(); }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::APN,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->apnDataChanged(); }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::Username,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->apnDataChanged(); }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::Password,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->apnDataChanged(); }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::AuthType,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->apnDataChanged(); }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::ApnType,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->apnDataChanged(); }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::ApnProtocol,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [this]() { this->apnDataChanged(); }));

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void NewApnModel::clearData()
{
    list->clear();

    eraseInternalData();

    createData();

    list->rebuildList();
}

void NewApnModel::saveData(const std::shared_ptr<packet_data::APN::Config> &apnRecord)
{
    for (auto item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(apnRecord);
        }
    }
}

void NewApnModel::loadData(const std::shared_ptr<packet_data::APN::Config> &apnRecord)
{
    for (auto item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(apnRecord);
        }
    }
}

void NewApnModel::apnDataChanged()
{
    for (auto item : internalData) {
        if (item->onEmptyCallback && !item->onEmptyCallback()) {
            application->getCurrentWindow()->setBottomBarActive(gui::BottomBar::Side::CENTER, true); // SAVE button
            return;
        }
    }
    application->getCurrentWindow()->setBottomBarActive(gui::BottomBar::Side::CENTER, false);
}
