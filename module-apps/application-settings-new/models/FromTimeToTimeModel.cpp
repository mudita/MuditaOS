// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FromTimeToTimeModel.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "AppWindow.hpp"
#include <ListView.hpp>
#include <module-utils/time/DateAndTimeSettings.hpp>

FromTimeToTimeModel::FromTimeToTimeModel(app::Application *app) : application(app)
{}

void FromTimeToTimeModel::loadData(std::shared_ptr<EventsRecord> record)
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

void FromTimeToTimeModel::saveData(std::shared_ptr<EventsRecord> record)
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback(record);
        }
    }
}

gui::ListItem *FromTimeToTimeModel::getItem(gui::Order order)
{
    return getRecord(order);
}

unsigned int FromTimeToTimeModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::checkBox::height;
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
    auto app = application;
    assert(app != nullptr);

    fromTimeWidget = new gui::TimeWidget(
        utils::localize.get("app_settings_nightshift_from"),
        gui::TimeWidget::Type::Start,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    toTimeWidget = new gui::TimeWidget(
        utils::localize.get("app_settings_nightshift_to"),
        gui::TimeWidget::Type::End,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    fromTimeWidget->setConnectionToSecondItem(toTimeWidget);
    toTimeWidget->setConnectionToSecondItem(fromTimeWidget);

    internalData.push_back(fromTimeWidget);
    internalData.push_back(toTimeWidget);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}
