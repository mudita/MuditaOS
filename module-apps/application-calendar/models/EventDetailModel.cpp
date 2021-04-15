// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventDetailModel.hpp"
#include "application-calendar/widgets/EventDetailDescriptionItem.hpp"
#include "application-calendar/widgets/RepeatAndReminderItem.hpp"
#include <ListView.hpp>

EventDetailModel::EventDetailModel(app::Application *app) : application(app)
{}

auto EventDetailModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

unsigned int EventDetailModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::eventDetail::height_max;
}

void EventDetailModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

gui::ListItem *EventDetailModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void EventDetailModel::createData()
{
    auto app = dynamic_cast<app::ApplicationCalendar *>(application);
    assert(app != nullptr);

    internalData.push_back(new gui::EventDetailDescriptionItem());
    internalData.push_back(new gui::RepeatAndReminderItem(app));

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}

void EventDetailModel::loadData(std::shared_ptr<EventsRecord> record)
{
    list->reset();
    eraseInternalData();

    createData();

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback(record);
        }
    }

    list->rebuildList();
}
