// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationStyle.hpp"
#include "ReadyGoingWindow.hpp"

namespace gui
{
    ReadyGoingWindow::ReadyGoingWindow(
        app::ApplicationCommon *app,
        std::shared_ptr<app::meditation::ReadyGoingPresenterContract::Presenter> winPresenter)
        : WindowWithTimer(app, app::meditation::windows::readyGoing), presenter{std::move(winPresenter)}
    {
        const auto startDelay = presenter->getStartDelay();
        /// Even if the start delay is set to 0, give 'ReadyGoingWindow` 1s to display its contents
        resetTimer(startDelay == std::chrono::seconds{0} ? std::chrono::seconds{1} : startDelay);

        buildInterface();

        timerCallback = [this](Item &, sys::Timer &) {
            presenter->activate();
            return true;
        };
    }

    void ReadyGoingWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        if (icon == nullptr) {
            using namespace app::meditationStyle;
            icon = new Icon(this, 0, 0, style::window_width, style::window_height, {}, {});
            icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
            icon->image->setMargins({0, itStyle::icon::imageTopMargin, 0, itStyle::icon::imageBottomMargin});
            icon->image->set(app::meditationStyle::itStyle::icon::imageLogo, ImageTypeSpecifier::W_G);
            icon->text->setFont(itStyle::text::font);
            icon->text->setRichText(utils::translate("app_bell_meditation_put_down_and_wait"));
        }
    }

    bool ReadyGoingWindow::onInput(const InputEvent &)
    {
        return true;
    }
} // namespace gui
