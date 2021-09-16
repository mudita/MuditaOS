// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-gui/gui/input/InputEvent.hpp>
#include <i18n/i18n.hpp>
#include "ApplicationCommon.hpp"
#include "VolumeWindow.hpp"
#include "popups/data/PopupData.hpp"

namespace gui
{
    VolumeWindow::VolumeWindow(app::ApplicationCommon *app, const std::string &name) : WindowWithTimer(app, name)
    {
        buildInterface();
    }

    void VolumeWindow::addVolumeText()
    {
        volumeText = new Label(this,
                               style::window::default_left_margin,
                               style::window::default_vertical_pos,
                               style::window::default_body_width,
                               style::window::volume::title_height,
                               utils::translate(style::window::volume::base_title_key));

        volumeText->setPenWidth(style::window::default_border_no_focus_w);
        volumeText->setFont(style::window::font::mediumbold);
        volumeText->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center));
        addWidget(volumeText);
    }

    void VolumeWindow::addVolumeBar()
    {
        volumeBar = new VBarGraph(this,
                                  style::window::default_left_margin,
                                  style::window::volume::bar::top_offset,
                                  style::window::volume::bar::volume_levels);
    }

    void VolumeWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        bottomBar->setVisible(false);
        statusBar->setVisible(false);

        addVolumeText();
        addVolumeBar();
    }

    void VolumeWindow::rebuild()
    {}

    void VolumeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);
        const auto popupData = dynamic_cast<VolumePopupData *>(data);
        if (popupData) {
            volume       = popupData->getVolume();
            audioContext = popupData->getAudioContext();
            if (volumeBar != nullptr) {
                volumeBar->setValue(volume);
            }
            if (volumeText != nullptr) {
                showProperText(audioContext, volume);
            }
        }
    }

    void VolumeWindow::showProperText(const audio::Context &audioContext, const audio::Volume volume) noexcept
    {
        volumeText->setText(utils::translate(style::window::volume::base_title_key));
        const auto [profileType, playbackType] = audioContext;
        if (playbackType == audio::PlaybackType::Multimedia) {
            showMultimediaPlayback();
        }
        else if (profileType == audio::Profile::Type::RoutingBluetoothHSP ||
                 profileType == audio::Profile::Type::RoutingEarspeaker ||
                 profileType == audio::Profile::Type::RoutingHeadphones ||
                 profileType == audio::Profile::Type::RoutingLoudspeaker) {
            showCalling();
        }

        if (volume == 0) {
            showMuted();
        }
    }

    void VolumeWindow::showMultimediaPlayback() noexcept
    {
        volumeText->setText(utils::translate(style::window::volume::music_title_key));
    }

    void VolumeWindow::showCalling() noexcept
    {
        volumeText->setText(utils::translate(style::window::volume::call_title_key));
    }

    void VolumeWindow::showMuted() noexcept
    {
        volumeText->setText(utils::translate(style::window::volume::mute_title_key));
    }

    bool VolumeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            return false;
        }

        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
