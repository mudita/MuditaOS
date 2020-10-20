// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DayEventsInternalModel.hpp"
#include "application-calendar/widgets/DayEventsItem.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "module-apps/application-calendar/ApplicationCalendar.hpp"
#include <ListView.hpp>
#include <algorithm>

DayEventsInternalModel::DayEventsInternalModel(app::Application *app) : application(app)
{}

unsigned int DayEventsInternalModel::requestRecordsCount()
{
    return internalData.size();
}

unsigned int DayEventsInternalModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::dayEvents::height;
}

void DayEventsInternalModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

gui::ListItem *DayEventsInternalModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void DayEventsInternalModel::loadData(std::unique_ptr<std::vector<EventsRecord>> records)
{
    auto app = dynamic_cast<app::ApplicationCalendar *>(application);
    assert(app != nullptr);

    list->clear();
    eraseInternalData();

    std::sort(records->begin(), records->end(), [](const EventsRecord &first, const EventsRecord &second) {
        return first.date_from < second.date_from;
    });

    auto eventShift = app->getEventShift();
    if (eventShift) {
        for (auto &record : *records) {
            record.date_from += hours(eventShift);
            record.date_till += hours(eventShift);
        }
    }

    for (auto &record : *records) {
        auto item = new gui::DayEventsItem();
        item->setEvent(std::make_shared<EventsRecord>(record));
        item->activatedCallback = [=](gui::Item &item) {
            auto rec  = std::make_unique<EventsRecord>(record);
            auto data = std::make_unique<EventRecordData>(std::move(rec));
            app->switchWindow(style::window::calendar::name::details_window, std::move(data));
            return true;
        };
        internalData.push_back(item);
    }

    for (auto &item : internalData) {
        item->deleteByList = false;
    }

    list->rebuildList();
}
