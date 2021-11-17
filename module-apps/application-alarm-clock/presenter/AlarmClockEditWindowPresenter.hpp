// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BasePresenter.hpp"
#include "application-alarm-clock/models/NewEditAlarmModel.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"

namespace app::alarmClock
{
    class AlarmClockEditWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<AlarmClockEditWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            [[nodiscard]] virtual std::shared_ptr<gui::ListItemProvider> getAlarmsItemProvider() const = 0;
            virtual void loadData(std::shared_ptr<AlarmEventRecord> record)                            = 0;
            virtual void saveData(std::shared_ptr<AlarmEventRecord> record, AlarmAction action)        = 0;
            virtual void loadCustomRepeat()                                                            = 0;
        };
    };

    class AlarmClockEditWindowPresenter : public AlarmClockEditWindowContract::Presenter
    {
      public:
        explicit AlarmClockEditWindowPresenter(std::shared_ptr<AlarmsInternalListItemProvider> itemProvider);

        [[nodiscard]] std::shared_ptr<gui::ListItemProvider> getAlarmsItemProvider() const override;
        void loadData(std::shared_ptr<AlarmEventRecord> record) override;
        void saveData(std::shared_ptr<AlarmEventRecord> record, AlarmAction action) override;
        void loadCustomRepeat() override;

      private:
        std::shared_ptr<AlarmsInternalListItemProvider> alarmFieldsProvider;
    };
} // namespace app::alarmClock
