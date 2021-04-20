// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SpecialInputMainWindow.hpp"
#include "../ApplicationSpecialInput.hpp"
#include "Style.hpp"
#include "messages/AppMessage.hpp"
#include <i18n/i18n.hpp>
#include <cassert>

using namespace gui;

SpecialInputMainWindow::SpecialInputMainWindow(app::Application *app)
    : AppWindow(app, app::char_select), model{std::make_shared<SpecialInputModel>(this->application)}
{
    buildInterface();
}

void SpecialInputMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    assert(dynamic_cast<app::ApplicationSpecialInput *>(application));
    model->clearData();
    actualWindow = specialInputStyle::CharactersType::Emoji;
    switchPage();
}
void SpecialInputMainWindow::rebuild()
{
    destroyInterface();
    buildInterface();
}
void SpecialInputMainWindow::buildInterface()
{
    AppWindow::buildInterface();

    bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
    bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::emoji));

    charList = new gui::ListView(this,
                                 specialInputStyle::specialInputListView::x,
                                 specialInputStyle::specialInputListView::y,
                                 specialInputStyle::specialInputListView::w,
                                 specialInputStyle::specialInputListView::h,
                                 model);
    setFocusItem(charList);
}

void SpecialInputMainWindow::destroyInterface()
{
    model->clearData();
    setFocusItem(nullptr);
    erase();
}

bool SpecialInputMainWindow::onInput(const InputEvent &inputEvent)
{
    auto ret = AppWindow::onInput(inputEvent);
    if (inputEvent.state == InputEvent::State::keyReleasedShort) {
        switch (inputEvent.keyCode) {
        case KeyCode::KEY_LF: {
            switchPage();
            return true;
        }
        default:
            break;
        }
    }
    return ret;
}

void SpecialInputMainWindow::switchPage()
{
    model->clearData();
    if (actualWindow == specialInputStyle::CharactersType::Emoji) {
        model->createData(specialInputStyle::CharactersType::SpecialCharacters);
        actualWindow = specialInputStyle::CharactersType::SpecialCharacters;
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::emoji));
        setTitle(utils::translate("app_special_input_window"));
    }
    else if (actualWindow == specialInputStyle::CharactersType::SpecialCharacters) {
        model->createData(specialInputStyle::CharactersType::Emoji);
        actualWindow = specialInputStyle::CharactersType::Emoji;
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::special_chars));
        setTitle(utils::translate("app_emoji_input_window"));
    }
}
