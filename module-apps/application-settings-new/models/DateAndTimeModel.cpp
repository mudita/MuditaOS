// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DateAndTimeModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "AppWindow.hpp"
#include <ListView.hpp>
#include <module-utils/time/DateAndTimeSettings.hpp>

DateAndTimeModel::DateAndTimeModel(app::Application *app) : application(app)
{}

void DateAndTimeModel::loadData(std::shared_ptr<EventsRecord> record)
{
    list->clear();
    eraseInternalData();

    createData();

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(record);
        }
    }

    list->rebuildList();
}

void DateAndTimeModel::saveData(std::shared_ptr<EventsRecord> record)
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(record);
        }
    }
}

gui::ListItem *DateAndTimeModel::getItem(gui::Order order)
{
    return getRecord(order);
}

unsigned int DateAndTimeModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::checkBox::height;
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
    auto app = application;
    assert(app != nullptr);

    dateItem = new gui::DateWidget();

    timeItem = new gui::TimeWidget(
        utils::localize.get("app_settings_title_time"),
        gui::TimeWidget::Type::Start,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    timeItem->setConnectionToDateItem(dateItem);

    internalData.push_back(dateItem);
    internalData.push_back(timeItem);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}
