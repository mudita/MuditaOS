/*
 * Window.cpp
 *
 *  Created on: 6 mar 2019
 *      Author: robert
 */
#include <algorithm>
// gui
#include "../core/BoundingBox.hpp"
#include "../Common.hpp"
#include "Window.hpp"
#include "../core/DrawCommand.hpp"

namespace gui
{
    Window::Window(std::string name) : Item(), refreshMode{RefreshModes::GUI_REFRESH_FAST}, name{name}
    {}

    void Window::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    void Window::onClose()
    {}

    void Window::getRefreshArea(RefreshModes &mode, uint16_t &x, uint16_t &y, uint16_t &w, uint16_t &h)
    {
        x    = widgetArea.x;
        y    = widgetArea.y;
        w    = widgetArea.w;
        h    = widgetArea.h;
        mode = refreshMode;
    }

    bool Window::handleSwitchData(SwitchData *data)
    {
        return true;
    }

    std::list<DrawCommand *> Window::buildDrawList()
    {

        std::list<DrawCommand *> commands;
        std::list<DrawCommand *> childrenCommands = Item::buildDrawList();

        DrawCommand *clearCommand = new DrawCommand();
        clearCommand->id          = DrawCommandID::GUI_DRAW_CLEAR;

        commands.push_back(clearCommand);

        if (!childrenCommands.empty()) {
            commands.splice(commands.end(), childrenCommands);
        }

        return commands;
    }

    bool Window::onInput(const InputEvent &inputEvent)
    {
        if (focusItem != nullptr && focusItem->onInput(inputEvent)) {
            return true;
        }
        if (inputCallback != nullptr && inputCallback(*this, inputEvent)) {
            return true;
        }
        if (handleNavigation(inputEvent)) {
            return true;
        }
        return inputEvent.state == InputEvent::State::keyReleasedShort &&
               inputEvent.keyCode == gui::KeyCode::KEY_ENTER && onActivated(nullptr);
    }

} /* namespace gui */
