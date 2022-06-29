// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
            virtual void updateAlarmModel(AlarmModelReadyHandler callback) = 0;
            virtual time_t getAlarmTime() const noexcept                   = 0;
            virtual bool isAlarmActive() const noexcept                    = 0;
            virtual void activate()                                        = 0;
            virtual void deactivate()                                      = 0;
        };
    };

    class AlarmActivatedPresenter : public AlarmActivatedContract::Presenter
    {
      public:
        AlarmActivatedPresenter(AbstractAlarmModel &alarmModel);

        void updateAlarmModel(AlarmModelReadyHandler callback);
        time_t getAlarmTime() const noexcept;
        bool isAlarmActive() const noexcept;
        void activate();
        void deactivate();

      private:
        AbstractAlarmModel &alarmModel;
    };
} // namespace app::popup
