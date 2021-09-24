// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "time/dateCommon.hpp"
#include <AlarmEventRecord.hpp>
#include <ApplicationCommon.hpp>
#include <apps-common/BasePresenter.hpp>
#include <memory>
#include <utility>

namespace app::popup
{
    class AlarmPopupPresenter;
    class AlarmPopupContract
    {
      public:
        class Presenter;

        class AlarmModel : public AsyncCallbackReceiver
        {
            friend Presenter;
            friend AlarmPopupPresenter;

            std::shared_ptr<AlarmEventRecord> record = nullptr;
            Presenter *presenter                     = nullptr;

            void set(std::shared_ptr<AlarmEventRecord> record)
            {
                this->record = std::move(record);
            }
            ApplicationCommon *app;
            void snoozeAlarm();
            void stopAlarm();

          public:
            explicit AlarmModel(ApplicationCommon *app) : AsyncCallbackReceiver(app), app(app)
            {}
            void attach(Presenter *p)
            {
                presenter = p;
            }
        };

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
            virtual ~View() noexcept        = default;
            virtual void showSnoozeButton() = 0;
        };

        class Presenter : public BasePresenter<AlarmPopupContract::View>
        {
            std::shared_ptr<AlarmModel> model;
            ApplicationCommon *app = nullptr;

          protected:
            std::shared_ptr<AlarmModel> &getModel()
            {
                return model;
            }

            ApplicationCommon *getApp()
            {
                return app;
            }

          public:
            explicit Presenter(ApplicationCommon *app) : model(std::make_shared<AlarmModel>(app)), app(app)
            {
                model->attach(this);
            }
            virtual ~Presenter() noexcept = default;
            virtual void setModel(std::shared_ptr<AlarmEventRecord> r)
            {
                this->model->set(std::move(r));
            }

            virtual std::uint32_t getSnoozeTime() = 0;
            virtual bool isSnoozeAble()           = 0;
            virtual void snoozeHit()              = 0;
            virtual void stopAlarm()              = 0;
            virtual void handleAlarmSnoozed()     = 0;
            virtual void handleAlarmTurnedOff()   = 0;
            virtual bool isSnoozed()              = 0;
            virtual std::string startedAt()       = 0;
            virtual std::string snoozedTill()     = 0;
        };
    };

    class AlarmPopupPresenter : public AlarmPopupContract::Presenter
    {
      public:
        explicit AlarmPopupPresenter(ApplicationCommon *app);

      private:
        bool noteChanged = false;
        /// returns how many seconds we can snooze the alarm
        virtual std::uint32_t getSnoozeTime() override;
        /// returns if alarm has snooze time available
        /// determines if we can snooze the alarm at all
        virtual bool isSnoozeAble() override;
        /// action on when user hit snooze button
        virtual void snoozeHit() override;
        /// action to stop alarm ringing
        virtual void stopAlarm() override;
        /// action to call when we processed snoozing the alarm
        virtual void handleAlarmSnoozed() override;
        /// action to call when we turned off alarm successfully
        virtual void handleAlarmTurnedOff() override;
        /// option to determine if alarm was snoozed previously and is recurring
        virtual bool isSnoozed() override;
        /// value for UI to show time when alarm started
        virtual std::string startedAt() override;
        /// value for UI to show if alarm `isSnoozed` till what hour it's snoozed exactly
        virtual std::string snoozedTill() override;
    };
} // namespace app::popup
