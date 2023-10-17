// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BasePresenter.hpp"
#include "application-alarm-clock/models/AlarmsModel.hpp"

namespace app::alarmClock
{
    class AlarmClockMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<AlarmClockMainWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            [[nodiscard]] virtual std::shared_ptr<gui::ListItemProvider> getAlarmsItemProvider() const = 0;
        };
    };

    class AlarmClockMainWindowPresenter : public AlarmClockMainWindowContract::Presenter
    {
      public:
        explicit AlarmClockMainWindowPresenter(std::shared_ptr<AlarmsListItemProvider> alarmsListItemProvider);

        [[nodiscard]] std::shared_ptr<gui::ListItemProvider> getAlarmsItemProvider() const override;

      private:
        std::shared_ptr<AlarmsListItemProvider> alarmsProvider;
    };
} // namespace app::alarmClock
