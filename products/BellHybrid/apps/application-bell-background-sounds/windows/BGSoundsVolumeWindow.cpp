// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsVolumeWindow.hpp"
#include <ApplicationBellBackgroundSounds.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <data/BGSoundsStyle.hpp>
#include <popups/data/PopupData.hpp>

namespace gui
{
    BGSoundsVolumeWindow::BGSoundsVolumeWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::bgSounds::AbstractBGSoundsVolumePresenter> &&presenter)
        : WindowWithTimer(app, gui::popup::window::volume_window), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void BGSoundsVolumeWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

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
        topMessage->setText(utils::translate("app_settings_volume"));
        topMessage->drawUnderline(false);

        auto data = presenter->getVolumeData();
        spinner   = new UIntegerSpinner({static_cast<UIntegerSpinner::Type>(data.min),
                                       static_cast<UIntegerSpinner::Type>(data.max),
                                       static_cast<UIntegerSpinner::Type>(data.step)},
                                      Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        spinner->setFont(bgSoundsStyle::valumeValueFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        spinner->setCurrentValue(static_cast<UIntegerSpinner::Type>(presenter->getVolume()));
        body->getCenterBox()->addWidget(spinner);

        spinner->onValueChanged = [this](const auto &value) { presenter->setVolume(value); };
        body->setMinMaxArrowsVisibility(spinner->getCurrentValue() == data.min, spinner->getCurrentValue() == data.max);

        setFocusItem(body);
        body->resize();
    }

    bool BGSoundsVolumeWindow::onInput(const InputEvent &inputEvent)
    {
        resetTimer();
        auto data              = presenter->getVolumeData();
        const auto ret         = body->onInput(inputEvent);
        const auto selectedVal = spinner->getCurrentValue();
        body->setMinMaxArrowsVisibility(selectedVal == data.min, selectedVal == data.max);
        return ret;
    }
} // namespace gui
