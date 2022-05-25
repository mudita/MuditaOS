// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationStyle.hpp"
#include "MeditationTimerWindow.hpp"

namespace app::meditation
{
    using namespace gui;
    MeditationTimerWindow::MeditationTimerWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::MeditationTimerContract::Presenter> &&windowPresenter)
        : AppWindow(app, name), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void MeditationTimerWindow::buildInterface()
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
        topMessage->setText(utils::translate("app_bell_meditation_timer"));
        topMessage->drawUnderline(false);

        spinner = new UIntegerSpinner(
            UIntegerSpinner::range{presenter->getMinValue(), presenter->getMaxValue(), presenter->getStepValue()},
            gui::Boundaries::Fixed);
        spinner->onValueChanged = [this](const auto val) { this->onValueChanged(val); };
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        spinner->setFont(app::meditationStyle::mtStyle::text::font);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setEdges(RectangleEdge::None);
        spinner->setFocusEdges(RectangleEdge::None);
        body->getCenterBox()->addWidget(spinner);

        auto currentValue = presenter->getCurrentValue();
        spinner->set_value(currentValue);
        body->setMinMaxArrowsVisibility(currentValue == presenter->getMinValue(),
                                        currentValue == presenter->getMaxValue());

        bottomDescription = new Label(body->lastBox);
        bottomDescription->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        bottomDescription->setFont(app::meditationStyle::mtStyle::minute::font);
        bottomDescription->setEdges(RectangleEdge::None);
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setText(presenter->getTimeUnitName(spinner->value()));

        setFocusItem(spinner);
        body->resize();
    }

    bool MeditationTimerWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (spinner->onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->activate(spinner->value());
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void MeditationTimerWindow::onValueChanged(const std::uint32_t currentValue)
    {
        body->setMinMaxArrowsVisibility(currentValue == presenter->getMinValue(),
                                        currentValue == presenter->getMaxValue());
        bottomDescription->setText(presenter->getTimeUnitName(spinner->value()));
    }
} // namespace app::meditation
