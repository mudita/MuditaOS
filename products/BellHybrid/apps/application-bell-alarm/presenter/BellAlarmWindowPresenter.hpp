
// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>
#include <common/models/TimeModel.hpp>
#include <time/dateCommon.hpp>

namespace app::bell_alarm
{
    class BellAlarmWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
            virtual void setAlarmTime(time_t time)                          = 0;
            virtual time_t getAlarmTime() const                             = 0;
            virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
        };

        class Presenter : public BasePresenter<BellAlarmWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept   = default;
            virtual auto createData() -> void                                       = 0;
            virtual auto saveData() -> void = 0;
            virtual auto setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void = 0;
            virtual auto onBeforeShow() -> void                                     = 0;
        };
    };

    class BellAlarmWindowPresenter : public BellAlarmWindowContract::Presenter
    {
      public:
        BellAlarmWindowPresenter(std::shared_ptr<AbstractAlarmModel> alarmModel,
                                 std::shared_ptr<AbstractTimeModel> timeModel);
        auto createData() -> void override;
        auto saveData() -> void override;
        auto setTimeFormat(utils::time::Locale::TimeFormat fmt) -> void override;
        auto onBeforeShow() -> void override;

      private:
        std::shared_ptr<AbstractAlarmModel> alarmModel;
        std::shared_ptr<AbstractTimeModel> timeModel;
    };
} // namespace app::bell_alarm
