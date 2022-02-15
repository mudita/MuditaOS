// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "UITestWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <Font.hpp>
#include <i18n/i18n.hpp>

namespace gui
{

    UiTestWindow::UiTestWindow(app::ApplicationCommon *app) : AppWindow(app, "TEST_UI")
    {
        AppWindow::buildInterface();
        buildInterface();
        // prebuild
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        setTitle("UI TEST");
        text = new gui::Text(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             300);
        text->setEditMode(EditMode::Edit);
        text->setFont(style::window::font::medium);
        LOG_DEBUG(
            "----------------------------------------------------------------------------------------------------");
        text->addRichText("<p><text font='gt_pressura' color='12' size='30'>This</text><br>Text<text size='20' "
                          "weight='bold'> is rich </text><text color='3'>example</text></br></p>");
        LOG_DEBUG(
            "----------------------------------------------------------------------------------------------------");
        text->addText(
            TextBlock("!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~"
                      "ĄąĆćĘęŁłŃńÓóŚśŹźŻżÀàÂâÇçÉéÈèÊêËëÎîÏïÔôÙûÛùÜüŸÿÄäÖößÁáÍíÚúÑñ¿¡",
                      Font(27).raw(),
                      TextBlock::End::None));
        text->addText(
            TextBlock("↠😁😂😃emoji😁😂😃emoji😁😂😃", Font("dejavu_sans", 27, Font::Weight::Bold).raw(), TextBlock::End::None));
        text->addText(TextBlock("Use fallback font:↠😁😂😃emoji😁😂😃emoji😁😂😃", Font(27).raw(), TextBlock::End::None));
        text->addText(TextBlock("Add text,", Font(27).raw(), TextBlock::End::None));
        text->addText(TextBlock("Add text,", Font(27).raw(), TextBlock::End::None));
        text->addText(TextBlock("Add text,", Font(27).raw(), TextBlock::End::None));
        text->addText(TextBlock("Add text,", Font(27).raw(), TextBlock::End::None));
        text->addText(TextBlock("long press * ", Font(27, Font::Weight::Bold).raw(), TextBlock::End::None));
        text->addText(
            TextBlock("adds special char, long press # changes input", Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock("(and shows it in bottom bar) arrows walk", Font(27).raw(), TextBlock::End::Newline));
        text->addText(TextBlock("1 in text mode ", Font(27).raw(), TextBlock::End::None));
        text->addText(TextBlock("adds ", Font(46, Font::Weight::Light).raw(), TextBlock::End::None));
        text->addText(TextBlock("special chars too", Font(27).raw(), TextBlock::End::None));
        text->setInputMode(new InputMode(
            {InputMode::Abc, InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { navBarTemporaryMode(text); },
            [=]() { navBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        text->setPenFocusWidth(0);
        setFocusItem(text);
    }
} // namespace gui
