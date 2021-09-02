// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CustomRepeatModel.hpp"
#include "application-alarm-clock/widgets/CustomCheckBoxWithLabel.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include <ListView.hpp>

namespace app::alarmClock
{

    CustomRepeatModel::CustomRepeatModel(app::Application *app,
                                         std::shared_ptr<AbstractAlarmsRepository> alarmsRepository)
        : application(app), alarmsRepository{std::move(alarmsRepository)}
    {}

    unsigned int CustomRepeatModel::requestRecordsCount()
    {
        return internalData.size();
    }

    unsigned int CustomRepeatModel::getMinimalItemSpaceRequired() const
    {
        return style::alarmClock::window::item::checkBox::height;
    }

    void CustomRepeatModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    gui::ListItem *CustomRepeatModel::getItem(gui::Order order)
    {
        return getRecord(order);
    }

    void CustomRepeatModel::createData(const WeekDaysRepeatData &data)
    {
        for (auto const &[key, dayName] : gui::CustomCheckBoxWithLabel::weekDays) {
            internalData.push_back(new gui::CustomCheckBoxWithLabel(application, utils::translate(dayName), data));
        }

        for (auto &item : internalData) {
            item->deleteByList = false;
        }
    }

    void CustomRepeatModel::loadData(const WeekDaysRepeatData &data)
    {
        list->reset();
        eraseInternalData();

        createData(data);

        list->rebuildList();
    }

    std::vector<bool> CustomRepeatModel::getIsCheckedData()
    {
        std::vector<bool> isCheckedData;
        isCheckedData.reserve(internalData.size());
        for (const auto &item : internalData) {
            if (item->onContentChangedCallback) {
                isCheckedData.push_back(item->onContentChangedCallback());
            }
            else {
                isCheckedData.push_back(false);
            }
        }

        return isCheckedData;
    }

} // namespace app::alarmClock
