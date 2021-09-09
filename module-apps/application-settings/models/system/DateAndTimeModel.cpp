// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateAndTimeModel.hpp"

#include <application-settings/widgets/system/SettingsDateItem.hpp>
#include <application-settings/widgets/system/SettingsTimeItem.hpp>

#include <ListView.hpp>
#include <widgets/DateAndTimeStyle.hpp>

DateAndTimeModel::DateAndTimeModel(app::ApplicationCommon *application) : app(application)
{}

void DateAndTimeModel::loadData(std::shared_ptr<utils::time::FromTillDate> fromTillDate)
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

void DateAndTimeModel::saveData(std::shared_ptr<utils::time::FromTillDate> fromTillDate)
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(fromTillDate);
        }
    }
}

gui::ListItem *DateAndTimeModel::getItem(gui::Order order)
{
    return getRecord(order);
}

unsigned int DateAndTimeModel::getMinimalItemSpaceRequired() const
{
    return style::window::date_and_time::height;
}

void DateAndTimeModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

unsigned int DateAndTimeModel::requestRecordsCount()
{
    return internalData.size();
}

void DateAndTimeModel::createData()
{
    dateItem = new gui::SettingsDateItem();

    timeItem = new gui::SettingsTimeItem(
        utils::translate("app_settings_title_time"),
        gui::TimeWidget::Type::Start,
        [&](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [&]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    timeItem->setConnectionToDateItem(dateItem);

    internalData.push_back(dateItem);
    internalData.push_back(timeItem);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}
