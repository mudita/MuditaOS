// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>
#include <tags_fetcher/TagsFetcher.hpp>
#include <memory>
namespace app
{
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
            ~View()                   = default;
            virtual void onFinished() = 0;
            virtual void onPaused()   = 0;
        };

        class Presenter : public BasePresenter<BGSoundsProgressContract::View>
        {
          public:
            virtual void activate(const tags::fetcher::Tags &tags)                  = 0;
            virtual void stop()                                                     = 0;
            virtual void pause()                                                    = 0;
            virtual void resume()                                                   = 0;
            virtual void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&timer) = 0;
        };
    };

    class AbstractBGSoundsPlayer;

    class BGSoundsProgressPresenter : public BGSoundsProgressContract::Presenter
    {
        settings::Settings *settings = nullptr;
        AbstractBGSoundsPlayer &player;
        std::unique_ptr<app::TimerWithCallbacks> timer;

        void activate(const tags::fetcher::Tags &tags) override;
        void stop() override;
        void pause() override;
        void resume() override;
        void setTimer(std::unique_ptr<app::TimerWithCallbacks> &&_timer) override;

        void onFinished();

      public:
        BGSoundsProgressPresenter(settings::Settings *settings, AbstractBGSoundsPlayer &player);
    };
} // namespace app::bgSounds
