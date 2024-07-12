// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationPausedWindow.hpp"
#include <data/RelaxationStyle.hpp>
#include <ApplicationBellRelaxation.hpp>

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    RelaxationPausedWindow::RelaxationPausedWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::relaxation::RelaxationPausedContract::Presenter> &&presenter)
        : AppWindow(app, window::name::relaxationPaused), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void RelaxationPausedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->onBeforeShow();
        updateTime();
    }

    void RelaxationPausedWindow::buildInterface()
    {
        using namespace gui::relaxationStyle;

        AppWindow::buildInterface();
        statusBar->setVisible(false);

        auto mainVBox = new VBox(this, 0, 0, style::window_width, style::window_height);
        mainVBox->setEdges(RectangleEdge::None);

        clock = new BellStatusClock(mainVBox);
        clock->setMaximumSize(relStyle::clock::maxSizeX, relStyle::clock::maxSizeY);
        clock->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        clock->setMargins(Margins(0, relStyle::clock::marginTop, 0, 0));

        auto imageVBox = new VBox(mainVBox);
        imageVBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        imageVBox->setEdges(RectangleEdge::None);
        imageVBox->setMargins(Margins(0, relStyle::pauseIcon::marginTop, 0, 0));
        imageVBox->setMinimumSize(relStyle::pauseIcon::minSizeX, relStyle::pauseIcon::loopMinSizeY);
        new Image(imageVBox, relStyle::pauseIcon::image, ImageTypeSpecifier::W_G);
        imageVBox->resizeItems();

        auto relaxationText = new TextFixedSize(mainVBox);
        relaxationText->setMaximumSize(relStyle::bottomDescription::maxSizeX, relStyle::bottomDescription::maxSizeY);
        relaxationText->setFont(relStyle::bottomDescription::font);
        relaxationText->setMargins(Margins(0, relStyle::bottomDescription::pausedMarginTop, 0, 0));
        relaxationText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        relaxationText->setText(utils::translate("app_bellmain_relaxation"));
        relaxationText->drawUnderline(false);
        relaxationText->setVisible(true);
        relaxationText->activeItem = false;

        mainVBox->resizeItems();
    }

    bool RelaxationPausedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            application->returnToPreviousWindow();
            return true;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->switchWindow(name::window::main_window);
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void RelaxationPausedWindow::setTime(std::time_t newTime)
    {
        clock->setTime(newTime);
        clock->setTimeFormatSpinnerVisibility(true);
    }

    void RelaxationPausedWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        clock->setTimeFormat(fmt);
    }

    RefreshModes RelaxationPausedWindow::updateTime()
    {
        if (presenter) {
            presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_FAST;
    }
} // namespace gui
