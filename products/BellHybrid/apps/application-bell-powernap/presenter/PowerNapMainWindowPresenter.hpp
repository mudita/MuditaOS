// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <memory>

namespace gui
{
    class ListItemProvider;
}

namespace app
{
    class Application;
}

namespace settings
{
    class Settings;
}

namespace app::powernap
{
    class PowerNapModel;
    class PowerNapMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };

        class Presenter : public BasePresenter<PowerNapMainWindowContract::View>
        {
          public:
            virtual auto getNapTimeProvider() -> std::shared_ptr<gui::ListItemProvider> = 0;

            virtual void loadNapTimeList() = 0;
            virtual void activate()        = 0;
        };
    };

    class PowerNapMainWindowPresenter : public PowerNapMainWindowContract::Presenter
    {
        app::Application *app        = nullptr;
        settings::Settings *settings = nullptr;
        std::shared_ptr<PowerNapModel> model;

      public:
        PowerNapMainWindowPresenter(app::Application *app, settings::Settings *settings);
        auto getNapTimeProvider() -> std::shared_ptr<gui::ListItemProvider> override;

        void loadNapTimeList() override;
        void activate() override;
    };

} // namespace app::powernap
