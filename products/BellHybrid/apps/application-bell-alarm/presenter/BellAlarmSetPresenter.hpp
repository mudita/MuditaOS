// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>
#include <memory>

namespace app
{
    class ApplicationCommon;
}

namespace app::bell_alarm
{
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
            virtual time_t getAlarmTime() const noexcept = 0;
            virtual bool isAlarmActive() const noexcept  = 0;
            virtual void activate()                      = 0;
        };
    };

    class BellAlarmSetPresenter : public BellAlarmSetContract::Presenter
    {
      public:
        explicit BellAlarmSetPresenter(app::ApplicationCommon *app, AbstractAlarmModel &alarmModel);

        time_t getAlarmTime() const noexcept;
        bool isAlarmActive() const noexcept;
        void activate() override;

      private:
        app::ApplicationCommon *app{};

        AbstractAlarmModel &alarmModel;
    };
} // namespace app::bell_alarm
