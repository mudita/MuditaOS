// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "MeditationStyle.hpp"
#include "ReadyGoingWindow.hpp"

namespace
{
    constexpr inline auto defaultTimeout = std::chrono::seconds{10};
}

namespace gui
{
    ReadyGoingWindow::ReadyGoingWindow(
        app::ApplicationCommon *app,
        std::shared_ptr<app::meditation::ReadyGoingPresenterContract::Presenter> winPresenter)
        : WindowWithTimer(app, gui::name::window::readyGoing, defaultTimeout), presenter{std::move(winPresenter)}
    {
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

    bool ReadyGoingWindow::onInput(const InputEvent &inputEvent)
    {
        return true;
    }
} // namespace gui
