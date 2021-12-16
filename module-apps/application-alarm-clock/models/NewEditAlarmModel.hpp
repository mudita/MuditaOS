// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-alarm-clock/widgets/AlarmInternalListItem.hpp>
#include <application-alarm-clock/models/AlarmsRepository.hpp>
#include <application-alarm-clock/data/AlarmsData.hpp>
#include <application-alarm-clock/presenter/AlarmRRulePresenter.hpp>
#include <apps-common/audio/SoundsPlayer.hpp>

#include <Application.hpp>
#include <InternalModel.hpp>
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
        virtual void clearData()                                                            = 0;
        virtual void loadCustomRepeat()                                                     = 0;
    };

    class NewEditAlarmModel : public AlarmsInternalListItemProvider
    {
        app::ApplicationCommon *application = nullptr;
        std::shared_ptr<AbstractAlarmsRepository> alarmsRepository;
        std::shared_ptr<SoundsPlayer> soundsPlayer = nullptr;
        std::shared_ptr<alarmClock::AlarmRRulePresenter> rRulePresenter;
        gui::AlarmInternalListItem *repeatOption = nullptr;

      public:
        NewEditAlarmModel(app::ApplicationCommon *app,
                          std::shared_ptr<SoundsPlayer> player,
                          std::shared_ptr<alarmClock::AlarmRRulePresenter> rRulePresenter,
                          std::shared_ptr<AbstractAlarmsRepository> alarmsRepository);

        void loadData(std::shared_ptr<AlarmEventRecord> record) override;
        void saveData(std::shared_ptr<AlarmEventRecord> alarm, AlarmAction action) override;
        void loadCustomRepeat() override;
        void clearData() override;
        void createData();

        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;
        [[nodiscard]] unsigned int requestRecordsCount() override;
        [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;
        void requestRecords(uint32_t offset, uint32_t limit) override;
    };
} // namespace app::alarmClock
