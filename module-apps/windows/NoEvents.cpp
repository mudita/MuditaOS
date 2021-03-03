// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoEvents.hpp"
#include "Dialog.hpp"
#include "DialogMetadataMessage.hpp"
#include "log/log.hpp"

#include <service-appmgr/Controller.hpp>
#include <Image.hpp>

using namespace gui;

namespace style
{
    namespace arrow
    {
        const inline uint32_t x = 30;
        const inline uint32_t y = 62;
    } // namespace arrow

    namespace cross
    {
        const inline uint32_t x = 48;
        const inline uint32_t y = 55;
    } // namespace cross

    namespace icon
    {
        constexpr inline auto x = 176;
        constexpr inline auto y = 195;
    } // namespace icon

    namespace text
    {
        constexpr inline auto x = 40;
        constexpr inline auto y = 333;
    } // namespace text
} // namespace style

NoEvents::NoEvents(app::Application *app, const std::string &name) : gui::Dialog(app, name)
{
    arrow = new gui::Image(this, style::arrow::x, style::arrow::y, 0, 0, "arrow_left");
    cross = new gui::Image(this, style::cross::x, style::cross::y, 0, 0, "cross");

    icon->setPosition(style::icon::x, style::icon::y);
    text->setPosition(style::text::x, style::text::y);
}

void NoEvents::onBeforeShow(ShowMode mode, SwitchData *data)
{
    Dialog::onBeforeShow(mode, data);
    auto metadata = dynamic_cast<DialogMetadataMessage *>(data);
    if (metadata != nullptr) {
        auto foo      = metadata->get().action;
        inputCallback = [foo](Item &, const InputEvent &inputEvent) -> bool {
            if (foo && inputEvent.isShortPress() && inputEvent.is(KeyCode::KEY_LEFT)) {
                return foo();
            }
            return false;
        };
    }

    if (title->getText() == utils::localize.get("app_calendar_title_main")) {
        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);
        bottomBar->setText(gui::BottomBar::Side::LEFT, utils::localize.get("app_calendar_bar_month"));
    }
}

bool NoEvents::onInput(const gui::InputEvent &inputEvent)
{
    if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_RF) &&
        title->getText() == utils::localize.get("app_calendar_title_main")) {
        app::manager::Controller::switchBack(application);
        return true;
    }

    if (AppWindow::onInput(inputEvent)) {
        return true;
    }

    if (inputEvent.isShortPress() && inputEvent.is(gui::KeyCode::KEY_LF) &&
        title->getText() == utils::localize.get("app_calendar_title_main")) {
        application->switchWindow(gui::name::window::main_window);
        return true;
    }

    return false;
}
