// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>
#include <common/models/BatteryModel.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::bell_alarm
{
    enum class Layout
    {
        Undefined,
        LowBatteryInfo,
        AlarmInfo,
        AlarmInactive
    };

    class BellAlarmSetContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };
        class Presenter : public BasePresenter<BellAlarmSetContract::View>
        {
          public:
            virtual std::time_t getAlarmTime() const noexcept = 0;
            virtual bool isAlarmActive() const noexcept  = 0;
            virtual void activate()                      = 0;
            virtual Layout getLayout()                   = 0;
            virtual void lowBatteryInfoHandled()         = 0;
        };
    };

    class BellAlarmSetPresenter : public BellAlarmSetContract::Presenter
    {
      public:
        BellAlarmSetPresenter(app::ApplicationCommon *app,
                              AbstractAlarmModel &alarmModel,
                              AbstractBatteryModel &batteryModel);

        std::time_t getAlarmTime() const noexcept override;
        bool isAlarmActive() const noexcept override;
        void activate() override;
        Layout getLayout() override;
        void lowBatteryInfoHandled() override;

      private:
        app::ApplicationCommon *app{};

        AbstractAlarmModel &alarmModel;
        AbstractBatteryModel &batteryModel;
        Layout layout{Layout::Undefined};
    };
} // namespace app::bell_alarm
