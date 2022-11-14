// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <common/models/BatteryModel.hpp>
#include <module-utils/EventStore/EventStore.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>
#include <service-db/Settings.hpp>
#include <time/time_locale.hpp>

namespace app
{
    class AbstractTimeModel;
    class AbstractBatteryModel;
    class ApplicationCommon;
} // namespace app
namespace gui
{
    class Item;
} // namespace gui
namespace settings
{
    class Settings;
}

namespace app::relaxation
{
    class RelaxationRunningLoopContract
    {
      public:
        class View
        {
          public:
            virtual ~View()                                                 = default;
            virtual void onFinished()                                       = 0;
            virtual void onPaused()                                         = 0;
            virtual void resume()                                           = 0;
            virtual void setTime(std::time_t newTime)                       = 0;
            virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
        };

        class Presenter : public BasePresenter<RelaxationRunningLoopContract::View>
        {
          public:
            virtual void activate(const db::multimedia_files::MultimediaFilesRecord &tags) = 0;
            virtual void stop()                                                            = 0;
            virtual void pause()                                                           = 0;
            virtual void resume()                                                          = 0;
            virtual bool isTimerStopped()                                                  = 0;
            virtual void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&timer)        = 0;
            virtual void handleUpdateTimeEvent()                                           = 0;
            virtual bool isPaused()                                                        = 0;
            virtual void onBeforeShow()                                                    = 0;
            virtual Store::Battery handleBatteryStatus()                                   = 0;
        };
    };

    class AbstractRelaxationPlayer;

    class RelaxationRunningLoopPresenter : public RelaxationRunningLoopContract::Presenter
    {
        settings::Settings *settings = nullptr;
        AbstractRelaxationPlayer &player;
        AbstractBatteryModel &batteryModel;
        std::unique_ptr<app::TimerWithCallbacks> timer;
        std::unique_ptr<AbstractTimeModel> timeModel;

        void activate(const db::multimedia_files::MultimediaFilesRecord &tags) override;
        void stop() override;
        void pause() override;
        void resume() override;
        bool isTimerStopped() override;
        void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) override;
        void handleUpdateTimeEvent() override;
        bool isPaused() override;
        void onBeforeShow() override;
        Store::Battery handleBatteryStatus() override;

        void onFinished();

      public:
        RelaxationRunningLoopPresenter(settings::Settings *settings,
                                       AbstractRelaxationPlayer &player,
                                       AbstractBatteryModel &batteryModel,
                                       std::unique_ptr<AbstractTimeModel> timeModel);
    };
} // namespace app::relaxation
