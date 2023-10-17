// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "time/dateCommon.hpp"
#include "Timers/TimerFactory.hpp"
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

          private:
            void set(std::shared_ptr<AlarmEventRecord> record);
            void setSnoozed(std::vector<SingleEventRecord> snoozed);
            void reset();
            template <typename requestType, typename responseType>
            void snoozeAlarm();
            template <typename requestType, typename responseType>
            void stopAlarm();
            void setRefreshWindowCallback(std::function<void()> callback);
            void processIfSnoozed();

            std::shared_ptr<AlarmEventRecord> record = nullptr;
            std::vector<SingleEventRecord> snoozedRecord;
            Presenter *presenter                        = nullptr;
            std::function<void()> refreshWindowCallback = nullptr;
            std::string snoozedTill                     = std::string{};
            bool isSnoozedAlarm                         = false;
            ApplicationCommon *app                      = nullptr;

          public:
            explicit AlarmModel(ApplicationCommon *app);
            void attach(Presenter *p);
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
        };

        class Presenter : public BasePresenter<AlarmPopupContract::View>
        {
          private:
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
            virtual void setSnoozedRecord(std::vector<SingleEventRecord> snoozed)
            {
                this->model->setSnoozed(snoozed);
            }
            virtual void setRefreshWindowCallback(std::function<void()> callback)
            {
                this->model->setRefreshWindowCallback(std::move(callback));
            }
            virtual void processIfSnoozed()
            {
                this->model->processIfSnoozed();
            }
            virtual void resetModel()
            {
                this->model->reset();
            }

            virtual std::uint32_t getSnoozeTime() = 0;
            virtual bool isSnoozeAble()           = 0;
            virtual void snoozeHit()              = 0;
            virtual void stopAlarm()              = 0;
            virtual void skipToNextSnooze()       = 0;
            virtual void handleAlarmSnoozed()     = 0;
            virtual void handleAlarmTurnedOff()   = 0;
            virtual bool isSnoozed()              = 0;
            virtual bool haveSnoozedSkip()        = 0;
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
        sys::TimerHandle timerHandle{};
        /// returns how many seconds we can snooze the alarm
        virtual std::uint32_t getSnoozeTime() override;
        /// returns if alarm has snooze time available
        /// determines if we can snooze the alarm at all
        bool isSnoozeAble() override;
        /// action on when user hit snooze button
        void snoozeHit() override;
        /// action to stop alarm ringing
        void stopAlarm() override;
        /// action to skip to next snooze
        void skipToNextSnooze() override;
        /// action to call when we processed snoozing the alarm
        void handleAlarmSnoozed() override;
        /// action to call when we turned off alarm successfully
        void handleAlarmTurnedOff() override;
        /// option to determine if alarm was snoozed previously and is recurring
        bool isSnoozed() override;
        /// check if curent view should have option to skip alarm notification
        bool haveSnoozedSkip() override;
        /// value for UI to show time when alarm started
        std::string startedAt() override;
        /// value for UI to show if alarm `isSnoozed` till what hour it's snoozed exactly
        std::string snoozedTill() override;
    };
} // namespace app::popup
