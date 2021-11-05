// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <time/time_locale.hpp>
#include <memory>
namespace app
{
    class AbstractTimeModel;
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

namespace app::bgSounds
{
    class BGSoundsProgressContract
    {
      public:
        class View
        {
          public:
            virtual ~View()                                                 = default;
            virtual void onFinished()                                       = 0;
            virtual void onPaused()                                         = 0;
            virtual void setTime(std::time_t newTime)                       = 0;
            virtual void setTimeFormat(utils::time::Locale::TimeFormat fmt) = 0;
        };

        class Presenter : public BasePresenter<BGSoundsProgressContract::View>
        {
          public:
            virtual void activate(const db::multimedia_files::MultimediaFilesRecord &tags) = 0;
            virtual void stop()                                                            = 0;
            virtual void pause()                                                           = 0;
            virtual void resume()                                                          = 0;
            virtual void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&timer)        = 0;
            virtual void handleUpdateTimeEvent()                                           = 0;
            virtual bool isPaused()                                                        = 0;
            virtual void onBeforeShow()                                                    = 0;
        };
    };

    class AbstractBGSoundsPlayer;

    class BGSoundsProgressPresenter : public BGSoundsProgressContract::Presenter
    {
        settings::Settings *settings = nullptr;
        AbstractBGSoundsPlayer &player;
        std::unique_ptr<app::TimerWithCallbacks> timer;
        std::unique_ptr<AbstractTimeModel> timeModel;

        void activate(const db::multimedia_files::MultimediaFilesRecord &tags) override;
        void stop() override;
        void pause() override;
        void resume() override;
        void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) override;
        void handleUpdateTimeEvent() override;
        bool isPaused() override;
        void onBeforeShow() override;

        void onFinished();

      public:
        BGSoundsProgressPresenter(settings::Settings *settings,
                                  AbstractBGSoundsPlayer &player,
                                  std::unique_ptr<AbstractTimeModel> timeModel);
    };
} // namespace app::bgSounds
