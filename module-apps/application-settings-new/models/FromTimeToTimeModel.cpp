// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FromTimeToTimeModel.hpp"
#include <application-settings-new/widgets/SettingsDateItem.hpp>
#include <application-settings-new/widgets/SettingsTimeItem.hpp>
#include <ListView.hpp>
#include <widgets/DateAndTimeStyle.hpp>

FromTimeToTimeModel::FromTimeToTimeModel(app::Application *application) : app(application)
{}

void FromTimeToTimeModel::loadData(std::shared_ptr<utils::time::FromTillDate> fromTillDate)
{
    list->reset();
    eraseInternalData();

    createData();

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(fromTillDate);
        }
    }

    list->rebuildList();
}

void FromTimeToTimeModel::saveData(std::shared_ptr<utils::time::FromTillDate> fromTillDate)
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(fromTillDate);
        }
    }
}

gui::ListItem *FromTimeToTimeModel::getItem(gui::Order order)
{
    return getRecord(order);
}

unsigned int FromTimeToTimeModel::getMinimalItemHeight() const
{
    return style::window::date_and_time::height;
}

void FromTimeToTimeModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

unsigned int FromTimeToTimeModel::requestRecordsCount()
{
    return internalData.size();
}

void FromTimeToTimeModel::createData()
{
    fromTimeItem = new gui::SettingsTimeItem(
        utils::translate("app_settings_nightshift_from"),
        gui::TimeWidget::Type::Start,
        [&](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [&]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    toTimeItem = new gui::SettingsTimeItem(
        utils::translate("app_settings_nightshift_to"),
        gui::TimeWidget::Type::End,
        [&](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [&]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    fromTimeItem->setConnectionToSecondItem(toTimeItem);
    toTimeItem->setConnectionToSecondItem(fromTimeItem);

    internalData.push_back(fromTimeItem);
    internalData.push_back(toTimeItem);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}
