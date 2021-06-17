// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/Arc.hpp>
#include "BrightnessWindow.hpp"

namespace gui
{
    BrightnessWindow::BrightnessWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void BrightnessWindow::addBox()
    {
        border = new Rect(this,
                          0,
                          style::window::brightness::box::top_offset,
                          style::window::brightness::box::width,
                          style::window::brightness::box::height);
        border->setEdges(RectangleEdge::Top);
    }

    void BrightnessWindow::addBrightnessText()
    {
        brightnessText = new BrightnessBox(
            this, style::window::brightness::title::left_offset, style::window::brightness::title::top_offset);
    }

    void BrightnessWindow::addBrightnessBar()
    {
        brightnessBar = new HBarGraph(this,
                                      style::window::brightness::bar::left_offset + 20,
                                      style::window::brightness::bar::top_offset,
                                      style::window::brightness::bar::brightness_levels);
    }

    void BrightnessWindow::buildInterface()
    {
        AppWindow::buildInterface();
        addBox();
        addBrightnessText();
        addBrightnessBar();
    }

    void BrightnessWindow::rebuild()
    {}

    void BrightnessWindow::destroyInterface()
    {
        erase();
    }

    BrightnessWindow::~BrightnessWindow()
    {
        destroyInterface();
    }

    void BrightnessWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    bool BrightnessWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (!inputEvent.isShortRelease()) {
            return false;
        }

        int update = brightnessBar->getValue();
        if (inputEvent.is(gui::KeyCode::KEY_VOLUP)) {
            update++;
        }

        if (inputEvent.is(gui::KeyCode::KEY_VOLDN)) {
            update--;
        }

        if (update >= 0) {
            brightnessBar->setValue(update);
        }

        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
