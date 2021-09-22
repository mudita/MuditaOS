// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumeWindow.hpp"
#include <ApplicationBellBackgroundSounds.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <data/BGSoundsStyle.hpp>

namespace gui
{
    BGSoundsVolumeWindow::BGSoundsVolumeWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bgSounds::BGSoundsVolumeContract::Presenter> &&presenter)
        : WindowWithTimer(app, gui::window::name::bgSoundsVolume), presenter{std::move(presenter)}
    {
        buildInterface();
        this->presenter->attach(this);
    }

    void BGSoundsVolumeWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, true);

        auto topMessage = new TextFixedSize(body->firstBox);
        topMessage->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topMessage->setFont(style::window::font::largelight);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setText(utils::translate("app_settings_volume"));
        topMessage->drawUnderline(false);

        auto data = presenter->getVolumeData();
        spinner   = new UIntegerSpinner({data.min, data.max, data.step}, Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        spinner->setFont(bgSoundsStyle::valumeValueFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        spinner->setCurrentValue(presenter->getCurrentVolume());
        body->getCenterBox()->addWidget(spinner);

        setFocusItem(spinner);
        body->resize();
    }

    bool BGSoundsVolumeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        resetTimer();
        if (spinner->onInput(inputEvent)) {
            auto currentVolume = spinner->getCurrentValue();
            presenter->onVolumeChanged(currentVolume);

            auto isMax = currentVolume == presenter->getVolumeData().max;
            auto isMin = currentVolume == presenter->getVolumeData().min;
            body->setArrowVisible(BellBaseLayout::Arrow::Left, isMin);
            body->setArrowVisible(BellBaseLayout::Arrow::Right, isMax);

            return true;
        }
        return WindowWithTimer::onInput(inputEvent);
    }

} // namespace gui
