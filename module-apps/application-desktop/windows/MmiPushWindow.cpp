﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MmiPushWindow.hpp"
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <application-desktop/data/Mmi.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

// move to style
namespace style::desktop
{
    namespace image
    {
        constexpr uint32_t x = 176;
        constexpr uint32_t y = 135;
    } // namespace image
    namespace text
    {
        constexpr uint32_t x = 40;
        constexpr uint32_t y = 300;
        constexpr uint32_t w = 400;
        constexpr uint32_t h = 300;
    } // namespace text

} // namespace style::desktop

MmiPushWindow::MmiPushWindow(app::Application *app, const std::string &name) : gui::AppWindow(app, name)
{
    AppWindow::buildInterface();
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::ok));
    icon = new Image(this, style::desktop::image::x, style::desktop::image::y, "");
    icon->set("info_big_circle_W_G");
    text = new Text(
        this, style::desktop::text::x, style::desktop::text::y, style::desktop::text::w, style::desktop::text::h);
    text->setTextType(TextType::MultiLine);
    text->setEditMode(EditMode::Browse);
    text->setEdges(RectangleEdge::None);
    text->setFont(style::window::font::medium);
    text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    setTitle(utils::localize.get("app_desktop_info"));
}

void MmiPushWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<app::manager::actions::MMIParams *>(data); metadata != nullptr) {
        text->setText(mmi::removePhrase(metadata->getData(), "\r"));
    }
}

bool MmiPushWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortPress()) {
        switch (inputEvent.keyCode) {
        case KeyCode::KEY_ENTER: {
            application->returnToPreviousWindow();
            return true;
        }
        case KeyCode::KEY_RF: {
            return true;
        }
        default:
            break;
        }
    }
    return AppWindow::onInput(inputEvent);
}

void MmiPushWindow::destroyInterface()
{
    erase();
}

MmiPushWindow::~MmiPushWindow()
{
    destroyInterface();
}
