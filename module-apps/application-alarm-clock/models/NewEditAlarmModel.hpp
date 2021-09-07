// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-alarm-clock/widgets/AlarmInternalListItem.hpp"
#include "application-alarm-clock/models/AlarmsRepository.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "Application.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>

namespace app::alarmClock
{
    class AlarmsInternalListItemProvider : public InternalModel<gui::AlarmInternalListItem *>,
                                           public gui::ListItemProvider
    {
      public:
        AlarmsInternalListItemProvider() = default;

        virtual void loadData(std::shared_ptr<AlarmEventRecord> record)                     = 0;
        virtual void saveData(std::shared_ptr<AlarmEventRecord> record, AlarmAction action) = 0;
        virtual void loadRepeat(std::shared_ptr<AlarmEventRecord> record)                   = 0;
    };

    class NewEditAlarmModel : public AlarmsInternalListItemProvider
    {
        app::Application *application = nullptr;
        std::shared_ptr<AbstractAlarmsRepository> alarmsRepository;
        gui::AlarmInternalListItem *repeatOption = nullptr;
        bool mode24H = false;

      public:
        NewEditAlarmModel(app::Application *app,
                          std::shared_ptr<AbstractAlarmsRepository> alarmsRepository,
                          bool mode24H = false);

        void loadData(std::shared_ptr<AlarmEventRecord> record) override;
        void saveData(std::shared_ptr<AlarmEventRecord> alarm, AlarmAction action) override;
        void loadRepeat(std::shared_ptr<AlarmEventRecord> record) override;
        void createData();

        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;
        [[nodiscard]] unsigned int requestRecordsCount() override;
        [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;
        void requestRecords(uint32_t offset, uint32_t limit) override;
    };
} // namespace app::alarmClock
