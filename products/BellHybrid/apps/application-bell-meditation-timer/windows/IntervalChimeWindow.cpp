// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "IntervalChimeWindow.hpp"
#include "MeditationStyle.hpp"

namespace gui
{
    IntervalChimeWindow::IntervalChimeWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::IntervalChimeContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::intervalChime), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void IntervalChimeWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, true);

        auto topMessage = new TextFixedSize(body->firstBox);
        topMessage->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topMessage->setFont(style::window::font::largelight);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setText(utils::translate("app_bell_meditation_interval_chime"));
        topMessage->drawUnderline(false);

        auto titles             = presenter->getIntervals();
        spinner                 = new UTF8Spinner({titles.begin(), titles.end()}, Boundaries::Fixed);
        spinner->onValueChanged = [this](const auto val) { this->onValueChanged(val); };
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(app::meditationStyle::icStyle::text::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);

        auto currentValue = presenter->getCurrentInterval();
        spinner->setCurrentValue(currentValue);
        body->setMinMaxArrowsVisibility(currentValue == titles.front(), currentValue == titles.back());

        bottomDescription = new Label(body->lastBox);
        bottomDescription->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        bottomDescription->setFont(app::meditationStyle::icStyle::minute::font);
        bottomDescription->setEdges(RectangleEdge::None);
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setText(presenter->getTimeUnitName(spinner->getCurrentValue()));

        setFocusItem(spinner);
        body->resize();
    }

    bool IntervalChimeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (spinner->onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->activate(spinner->getCurrentValue());
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void IntervalChimeWindow::onValueChanged(const std::string currentValue)
    {
        auto titles = presenter->getIntervals();
        body->setMinMaxArrowsVisibility(currentValue == titles.front(), currentValue == titles.back());
        bottomDescription->setText(presenter->getTimeUnitName(spinner->getCurrentValue()));
    }
} // namespace gui
