#include "UITestWindow.hpp"
#include "../ApplicationSettings.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "i18/i18.hpp"
#include "messages/AppMessage.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <GridLayout.hpp>
#include <Style.hpp>
#include <functional>
#include <memory>
#include <Font.hpp>

namespace gui
{

    UiTestWindow::UiTestWindow(app::Application *app) : AppWindow(app, "TEST_UI")
    {
        AppWindow::buildInterface();
        buildInterface();
        // prebuild
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        setTitle("UI TEST");
        text = new gui::Text(this,
                             style::window::default_left_margin,
                             title->offset_h(),
                             style::window_width - 2 * style::window::default_left_margin,
                             300);
        text->setEditMode(EditMode::EDIT);
        text->setFont(style::window::font::medium);
        text->addText(
            TextBlock("!#$%&'()*+,-.0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~"
                      "ĄąĆćĘęŁłŃńÓóŚśŹźŻżÀàÂâÇçÉéÈèÊêËëÎîÏïÔôÙûÛùÜüŸÿÄäÖößÁáÍíÚúÑñ¿¡",
                      Font(27).raw(),
                      TextBlock::End::None));
        text->addText(
            TextBlock("😁emoji😂emoji😃", Font("dejavu_sans", 27, Font::Weight::Bold).raw(), TextBlock::End::None));
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
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { bottomBarTemporaryMode(text); },
            [=]() { bottomBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        text->setPenFocusWidth(0);
        setFocusItem(text);
    }

    void UiTestWindow::rebuild()
    {
        LOG_INFO("Only if I have to!");
    }

    void UiTestWindow::buildInterface()
    {}

    void UiTestWindow::destroyInterface()
    {
        erase();
    }

    void UiTestWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

} // namespace gui
