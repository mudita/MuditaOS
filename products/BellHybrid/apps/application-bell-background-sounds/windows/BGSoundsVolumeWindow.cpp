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
        spinner   = new U8IntegerSpinner({static_cast<U8IntegerSpinner::value_type>(data.min),
                                        static_cast<U8IntegerSpinner::value_type>(data.max),
                                        static_cast<U8IntegerSpinner::value_type>(data.step)},
                                       Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        spinner->setFont(bgSoundsStyle::valumeValueFont);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        spinner->set_value(static_cast<U8IntegerSpinner::value_type>(presenter->getVolume()));
        body->getCenterBox()->addWidget(spinner);

        spinner->onValueChanged = [this](const auto &value) { presenter->setVolume(value); };
        body->setMinMaxArrowsVisibility(spinner->value() == data.min, spinner->value() == data.max);

        setFocusItem(body);
        body->resize();
    }

    bool BGSoundsVolumeWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) || inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->returnToPreviousWindow();
            return true;
        }
        resetTimer();
        auto data              = presenter->getVolumeData();
        const auto ret         = body->onInput(inputEvent);
        const auto selectedVal = spinner->value();
        body->setMinMaxArrowsVisibility(selectedVal == data.min, selectedVal == data.max);
        return ret;
    }
} // namespace gui
