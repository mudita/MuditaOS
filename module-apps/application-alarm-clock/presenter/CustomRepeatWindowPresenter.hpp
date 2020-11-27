// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BasePresenter.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "application-alarm-clock/models/CustomRepeatModel.hpp"

namespace app::alarmClock
{
    class CustomRepeatWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<CustomRepeatWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            [[nodiscard]] virtual std::shared_ptr<gui::ListItemProvider> getItemProvider() = 0;
            virtual void loadData(const WeekDaysRepeatData &data)                          = 0;
            virtual WeekDaysRepeatData getWeekDaysRepeatData()                             = 0;
        };
    };

    class CustomRepeatWindowPresenter : public CustomRepeatWindowContract::Presenter
    {
      public:
        explicit CustomRepeatWindowPresenter(std::shared_ptr<CustomRepeatListItemProvider> itemProvider);

        [[nodiscard]] std::shared_ptr<gui::ListItemProvider> getItemProvider() override;
        void loadData(const WeekDaysRepeatData &data) override;
        WeekDaysRepeatData getWeekDaysRepeatData() override;

      private:
        std::shared_ptr<CustomRepeatListItemProvider> customRepeatProvider;
    };
} // namespace app::alarmClock
