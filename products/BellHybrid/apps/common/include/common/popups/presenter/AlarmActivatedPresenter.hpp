// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <common/models/AbstractAlarmModel.hpp>

#include <memory>

namespace app::popup
{
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
            [[nodiscard]] virtual std::time_t getAlarmTime() const noexcept = 0;
            [[nodiscard]] virtual bool isAlarmActive() const noexcept       = 0;
        };
    };

    class AlarmActivatedPresenter : public AlarmActivatedContract::Presenter
    {
      public:
        explicit AlarmActivatedPresenter(AbstractAlarmModel &alarmModel);

        [[nodiscard]] std::time_t getAlarmTime() const noexcept override;
        [[nodiscard]] bool isAlarmActive() const noexcept override;

      private:
        AbstractAlarmModel &alarmModel;
    };
} // namespace app::popup
