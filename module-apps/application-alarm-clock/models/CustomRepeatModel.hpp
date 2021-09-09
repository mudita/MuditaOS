// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-alarm-clock/widgets/AlarmInternalListItem.hpp"
#include "application-alarm-clock/models/AlarmsRepository.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "application-calendar/data/CalendarData.hpp"
#include "Application.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

namespace app::alarmClock
{
    class CustomRepeatListItemProvider : public InternalModel<gui::AlarmInternalListItem *>,
                                         public gui::ListItemProvider
    {
      public:
        CustomRepeatListItemProvider() = default;

        virtual void loadData(const WeekDaysRepeatData &data) = 0;
        virtual std::vector<bool> getIsCheckedData()          = 0;
    };

    class CustomRepeatModel : public CustomRepeatListItemProvider
    {
        app::ApplicationCommon *application = nullptr;
        std::shared_ptr<AbstractAlarmsRepository> alarmsRepository;
        void createData(const WeekDaysRepeatData &data);

      public:
        CustomRepeatModel(app::ApplicationCommon *app, std::shared_ptr<AbstractAlarmsRepository> alarmsRepository);

        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;
        [[nodiscard]] unsigned int requestRecordsCount() override;
        [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;
        void requestRecords(uint32_t offset, uint32_t limit) override;

        void loadData(const WeekDaysRepeatData &data) override;
        std::vector<bool> getIsCheckedData() override;
    };
} // namespace app::alarmClock
