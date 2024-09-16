// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "WhatsNewCommon.hpp"
#include <presenter/WhatsNewFeaturesPresenter.hpp>
#include <apps-common/windows/AppWindow.hpp>
#include <widgets/spinners/Spinners.hpp>

namespace app::whatsnew
{
    class WhatsNewFeaturesWindow : public gui::AppWindow, public WhatsNewFeaturesContract::View
    {
      public:
        WhatsNewFeaturesWindow(app::ApplicationCommon *app,
                               std::unique_ptr<WhatsNewFeaturesContract::Presenter> &&presenter,
                               const std::string &name = window::name::features);

        auto buildInterface() -> void override;
        auto onInput(const gui::InputEvent &inputEvent) -> bool override;

      private:
        std::unique_ptr<WhatsNewFeaturesContract::Presenter> presenter;
        gui::WidgetSpinner *itemSpinner{nullptr};
        std::vector<gui::Item *> layouts;

        auto isLastLayout() -> bool;
        auto switchToEndWindow() -> void;
    };
} // namespace app::whatsnew
