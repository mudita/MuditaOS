
// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>
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
            virtual void setAlarmTime(time_t time) = 0;
            virtual time_t getAlarmTime()          = 0;
        };

        class Presenter : public BasePresenter<BellAlarmWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept   = default;
            virtual auto createData() -> void = 0;
            virtual auto saveData() -> void = 0;
        };
    };

    class BellAlarmWindowPresenter : public BellAlarmWindowContract::Presenter
    {
      public:
        BellAlarmWindowPresenter(std::unique_ptr<AbstractAlarmModel> &&alarmModel);
        auto createData() -> void override;
        auto saveData() -> void override;

      private:
        std::unique_ptr<AbstractAlarmModel> alarmModel;
    };
} // namespace app::bell_alarm
