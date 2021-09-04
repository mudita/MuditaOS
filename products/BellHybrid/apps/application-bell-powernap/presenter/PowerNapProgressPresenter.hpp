// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>
#include <memory>
namespace app
{
    class Application;
} // namespace app
namespace gui
{
    class Item;
} // namespace gui
namespace settings
{
    class Settings;
}

namespace app::powernap
{
    class PowerNapProgressContract
    {
      public:
        class View
        {
          public:
            ~View() = default;
        };

        class Presenter : public BasePresenter<PowerNapProgressContract::View>
        {
          public:
            virtual void initTimer(gui::Item *parent)                              = 0;
            virtual app::ProgressTimerUIConfigurator &getUIConfigurator() noexcept = 0;
            virtual void activate()                                                = 0;
        };
    };

    class PowerNapProgressPresenter : public PowerNapProgressContract::Presenter
    {
        app::Application *app        = nullptr;
        settings::Settings *settings = nullptr;
        std::unique_ptr<app::ProgressTimer> timer;

        void initTimer(gui::Item *parent) override;
        void activate() override;
        app::ProgressTimerUIConfigurator &getUIConfigurator() noexcept override;

      public:
        PowerNapProgressPresenter(app::Application *app, settings::Settings *settings);
    };
} // namespace app::powernap
