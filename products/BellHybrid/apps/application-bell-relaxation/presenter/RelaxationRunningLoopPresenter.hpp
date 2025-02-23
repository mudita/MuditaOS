// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
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
            virtual void onPaused()                                         = 0;
            virtual void resume()                                           = 0;
            virtual void setTime(std::time_t newTime)                       = 0;
            virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
            virtual void handleError()                                      = 0;
            virtual void handleDeletedFile()                                = 0;
        };

        class Presenter : public BasePresenter<RelaxationRunningLoopContract::View>
        {
          public:
            virtual void activate(const db::multimedia_files::MultimediaFilesRecord &tags) = 0;
            virtual void stop()                                                            = 0;
            virtual void pause()                                                           = 0;
            virtual void resume()                                                          = 0;
            virtual void handleUpdateTimeEvent()                                           = 0;
            virtual bool isPaused()                                                        = 0;
            virtual void onBeforeShow()                                                    = 0;
            [[nodiscard]] virtual Store::Battery getBatteryState() const                    = 0;
            [[nodiscard]] virtual bool isBatteryCharging(Store::Battery::State state) const = 0;
        };
    };

    class AbstractRelaxationPlayer;

    class RelaxationRunningLoopPresenter : public RelaxationRunningLoopContract::Presenter
    {
      public:
        RelaxationRunningLoopPresenter(settings::Settings *settings,
                                       AbstractRelaxationPlayer &player,
                                       AbstractBatteryModel &batteryModel,
                                       std::unique_ptr<AbstractTimeModel> timeModel);

      private:
        settings::Settings *settings{nullptr};
        AbstractRelaxationPlayer &player;
        AbstractBatteryModel &batteryModel;
        std::unique_ptr<AbstractTimeModel> timeModel;

        void activate(const db::multimedia_files::MultimediaFilesRecord &tags) override;
        void stop() override;
        void pause() override;
        void resume() override;
        void handleUpdateTimeEvent() override;
        bool isPaused() override;
        void onBeforeShow() override;
        [[nodiscard]] Store::Battery getBatteryState() const override;
        [[nodiscard]] bool isBatteryCharging(Store::Battery::State state) const override;

        void onFinished();
    };
} // namespace app::relaxation
