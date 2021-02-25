// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-gui/gui/input/InputEvent.hpp>
#include <i18n/i18n.hpp>
#include "VolumeWindow.hpp"

namespace gui
{
    VolumeWindow::VolumeWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void VolumeWindow::addVolumeText()
    {
        volumeText = new Label(this,
                               style::window::default_left_margin,
                               title->offset_h(),
                               style::window::default_body_width,
                               style::window::volume::title_height,
                               utils::localize.get(style::window::volume::title_key));

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
        AppWindow::buildInterface();
        addVolumeText();
        addVolumeBar();
    }

    void VolumeWindow::rebuild()
    {}

    void VolumeWindow::destroyInterface()
    {
        erase();
    }

    VolumeWindow::~VolumeWindow()
    {
        destroyInterface();
    }

    void VolumeWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        updateGraph();
    }

    bool VolumeWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (!inputEvent.isShortPress()) {
            return false;
        }

        if ((inputEvent.isShortPress())) {
            switch (inputEvent.keyCode) {
            case KeyCode::KEY_VOLUP: {
                application->increaseCurrentVolume();
                updateGraph();
                return true;
            }
            case KeyCode::KEY_VOLDN: {
                application->decreaseCurrentVolume();
                updateGraph();
                return true;
            }
            case KeyCode::KEY_RF: {
                application->returnToPreviousWindow();
                return true;
            }
            default:
                break;
            }
        }

        return AppWindow::onInput(inputEvent);
    }

    void VolumeWindow::updateGraph()
    {
        application->getCurrentVolume(Volume);
        volumeBar->setValue(Volume);
    }
} // namespace gui
