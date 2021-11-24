// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <presenter/advanced/AboutYourBellWindowPresenter.hpp>

#include <apps-common/windows/AppWindow.hpp>
#include <ListView.hpp>

namespace gui
{
    class AboutYourBellWindow : public AppWindow, public app::bell_settings::AboutYourBellWindowContract::View
    {
      public:
        static constexpr auto name = "AboutYourBellWindow";
        AboutYourBellWindow(app::ApplicationCommon *app,
                            std::shared_ptr<app::bell_settings::AboutYourBellWindowContract::Presenter> presenter);

      private:
        static constexpr auto height     = 400;
        static constexpr auto width      = 380;
        static constexpr auto top_margin = 41;

        void buildInterface() override;

        ListView *list{};
        std::shared_ptr<app::bell_settings::AboutYourBellWindowContract::Presenter> presenter;
    };
} // namespace gui
