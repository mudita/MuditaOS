﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DesktopInputWidget.hpp"
#include "Mmi.hpp"
#include "MmiPullWindow.hpp"
#include "Names.hpp"

#include <i18n/i18n.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

#include <string>

using namespace gui;

// move to style
namespace style::desktop
{
    namespace text
    {
        constexpr std::uint32_t x = 30;
        constexpr std::uint32_t y = 120;
        constexpr std::uint32_t w = 440;
        constexpr std::uint32_t h = 320;
    } // namespace text

    namespace inputWidget
    {
        constexpr std::uint32_t x = 30;
        constexpr std::uint32_t y = 450;
        constexpr std::uint32_t w = 450;
        constexpr std::uint32_t h = 50;
    } // namespace inputWidget

} // namespace style::desktop

MmiPullWindow::MmiPullWindow(app::ApplicationCommon *app, const std::string &name) : gui::AppWindow(app, name)
{
    AppWindow::buildInterface();

    navBar->setText(nav_bar::Side::Center, utils::translate("app_desktop_replay"));
    navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
    text = new Text(
        this, style::desktop::text::x, style::desktop::text::y, style::desktop::text::w, style::desktop::text::h);
    text->setTextType(TextType::MultiLine);
    text->setEditMode(EditMode::Browse);
    text->setEdges(RectangleEdge::None);
    text->setFont(style::window::font::medium);
    text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
    setTitle(utils::translate("app_desktop_info"));

    InputBox = new DesktopInputWidget(application,
                                      this,
                                      style::desktop::inputWidget::x,
                                      style::desktop::inputWidget::y,
                                      style::desktop::inputWidget::w,
                                      style::desktop::inputWidget::h);

    InputBox->inputText->setInputMode(new InputMode(
        {InputMode::digit, InputMode::Abc, InputMode::ABC, InputMode::abc},
        [=](const UTF8 &text1) { navBarTemporaryMode(text1); },
        [=]() { navBarRestoreFromTemporaryMode(); },
        [=]() { selectSpecialCharacter(); },
        [=](std::function<void()> restoreFunction) {
            application->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
        }));

    addWidget(InputBox);
}

void MmiPullWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    auto metadata = dynamic_cast<app::manager::actions::MMIParams *>(data);
    if (metadata != nullptr) {
        text->setText(mmi::removePhrase(metadata->getData(), "\r"));
    }
    InputBox->setVisible(true);
    setFocusItem(InputBox->inputText);
}

bool MmiPullWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease()) {
        switch (inputEvent.getKeyCode()) {
        case KeyCode::KEY_RF: {
            CellularServiceAPI::USSDRequest(this->application, cellular::USSDMessage::RequestType::abortSession, "");
            application->switchWindow(app::window::name::desktop_main_window);
            return true;
        }
        default:
            break;
        }
    }
    return AppWindow::onInput(inputEvent);
}

void MmiPullWindow::destroyInterface()
{
    erase();
}

MmiPullWindow::~MmiPullWindow()
{
    destroyInterface();
}
