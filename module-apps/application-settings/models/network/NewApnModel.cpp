// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewApnModel.hpp"

#include <application-settings/data/SettingsItemData.hpp>
#include <application-settings/widgets/SettingsStyle.hpp>
#include <application-settings/widgets/network/ApnInputWidget.hpp>

#include <ListView.hpp>
#include <AppWindow.hpp>

NewApnModel::NewApnModel(app::ApplicationCommon *app) : application(app)
{}

auto NewApnModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto NewApnModel::getMinimalItemSpaceRequired() const -> unsigned int
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
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        },
        [this](const std::string &text) { this->apnNameChanged(text); }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::APN,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::Username,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::Password,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::AuthType,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::ApnType,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        }));

    internalData.emplace_back(new gui::ApnInputWidget(
        settingsInternals::ListItemName::ApnProtocol,
        [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); },
        [app](std::function<void()> restoreFunction) {
            app->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        }));

    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void NewApnModel::clearData()
{
    list->reset();

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

void NewApnModel::apnNameChanged(const std::string &newName)
{
    LOG_DEBUG("New APN name: %s", newName.c_str());
    const bool showSaveButton = !newName.empty();
    application->getCurrentWindow()->setNavBarActive(gui::nav_bar::Side::Center, showSaveButton);
}
