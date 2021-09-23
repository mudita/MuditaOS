// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <InputEvent.hpp>
#include <Image.hpp>
#include <Text.hpp>

#include "WithTimerWindow.hpp"
#include "MeditationBasePresenter.hpp"

namespace gui
{
    class IconTextWindow : public WithTimerWindow, public app::meditation::MeditationBaseContract::View
    {
      public:
        IconTextWindow(app::ApplicationCommon *app,
                       std::string name,
                       std::unique_ptr<app::meditation::MeditationBaseContract::Presenter> &&windowPresenter);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void buildInterface() override;
        void destroyInterface() override;
        void buildMeditationItem(MeditationItem &item) override;
        void onMeditationItemAvailable(MeditationItem *item) override;
        void updateDisplay() override;

        virtual std::chrono::seconds getTimeout() const = 0;
        virtual std::string getText()                   = 0;
        virtual std::string getImageName()              = 0;

      private:
        std::unique_ptr<app::meditation::MeditationBaseContract::Presenter> presenter;
        gui::Image *appImage = nullptr;
        gui::Text *text      = nullptr;

        void showImage();
        void showText();
    };
} // namespace gui
