// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>
#include <common/models/BatteryModel.hpp>

#include <memory>

namespace app::popup
{
    enum class Layout
    {
        Undefined,
        LowBatteryInfo,
        AlarmInfo
    };

    class AlarmActivatedContract
    {
      public:
        class Presenter;

        class View
        {
          private:
            std::shared_ptr<Presenter> presenter;

          protected:
            std::shared_ptr<Presenter> getPresenter()
            {
                return presenter;
            }

          public:
            explicit View(std::shared_ptr<Presenter> presenter) : presenter(std::move(presenter))
            {}

            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<AlarmActivatedContract::View>
        {
          public:
            virtual ~Presenter() noexcept                                  = default;
            virtual time_t getAlarmTime() const noexcept                   = 0;
            virtual bool isAlarmActive() const noexcept                    = 0;
            virtual Layout getLayout()                                     = 0;
            virtual void lowBatteryInfoHandled()                           = 0;
        };
    };

    class AlarmActivatedPresenter : public AlarmActivatedContract::Presenter
    {
      public:
        AlarmActivatedPresenter(AbstractAlarmModel &alarmModel, AbstractBatteryModel &batteryModel);

        time_t getAlarmTime() const noexcept;
        bool isAlarmActive() const noexcept;
        Layout getLayout();
        void lowBatteryInfoHandled();

      private:
        AbstractAlarmModel &alarmModel;
        AbstractBatteryModel &batteryModel;
        Layout layout{Layout::Undefined};
    };
} // namespace app::popup
