#include "Dialog.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <i18/i18.hpp>

using namespace gui;

namespace style
{
    namespace image
    {
        constexpr uint32_t x = 176;
        constexpr uint32_t y = 132;
    } // namespace image
    namespace text
    {
        constexpr uint32_t x = 40;
        constexpr uint32_t y = 290;
        constexpr uint32_t w = 400;
        constexpr uint32_t h = 132;
    } // namespace text
    namespace no
    {
        constexpr uint32_t x = 75;
        constexpr uint32_t y = 415;
        constexpr uint32_t w = 150;
        constexpr uint32_t h = 75;
    } // namespace no
    namespace yes
    {
        constexpr uint32_t x = 255;
        constexpr uint32_t y = 415;
        constexpr uint32_t w = 150;
        constexpr uint32_t h = 75;
    } // namespace yes
    namespace icontext
    {
        constexpr uint32_t x = image::x;
        constexpr uint32_t y = image::y + 40;
        constexpr uint32_t w = 130;
        constexpr uint32_t h = 100;
    } // namespace icontext

} // namespace style

Dialog::Dialog(app::Application *app, const std::string &name, const Dialog::Meta &meta)
    : gui::AppWindow(app, name), meta(meta)
{
    AppWindow::buildInterface();

    topBar->setActive(TopBar::Elements::TIME, true);
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

    setTitle(meta.title);

    icon = new Image(this, style::image::x, style::image::y, meta.icon);

    text = new Text(this, style::text::x, style::text::y, style::text::w, style::text::h);
    text->setText(meta.text);
    text->setTextType(TextType::MULTI_LINE);
    text->setEditMode(EditMode::BROWSE);
    text->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    text->setFont(style::window::font::medium);
    text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
}

void Dialog::update(const Meta &meta)
{
    this->meta = meta;
    setTitle(meta.title);
    text->setText(meta.text);
    icon->set(meta.icon);
    // meta.action not used
}

DialogConfirm::DialogConfirm(app::Application *app, const std::string &name, const Dialog::Meta &meta)
    : Dialog(app, name, meta)
{
    topBar->setActive(TopBar::Elements::BATTERY, true);
    topBar->setActive(TopBar::Elements::SIGNAL, true);

    bottomBar->setActive(BottomBar::Side::RIGHT, false);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::ok));
    setFocusItem(icon);
    icon->inputCallback = [=](Item &, const InputEvent &inputEvent) -> bool {
        if (inputEvent.state == InputEvent::State::keyReleasedShort && inputEvent.keyCode == gui::KeyCode::KEY_RF) {
            return true;
        }
        return false;
    };
    // Title not set
    setTitle("");
}

void DialogConfirm::update(const Meta &meta)
{
    Dialog::update(meta);

    icon->activatedCallback = [=](Item &) -> bool { return meta.action(); };
}

DialogYesNo::DialogYesNo(app::Application *app, const std::string &name, const Meta &meta) : Dialog(app, name, meta)
{
    no = new Label(
        this, style::no::x, style::no::y, style::no::w, style::no::h, utils::localize.get(style::strings::common::no));
    no->setPenWidth(0);
    no->setPenFocusWidth(3);
    no->setFilled(false);
    no->setBorderColor(ColorFullBlack);
    no->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    no->setFont(style::window::font::big);
    no->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    no->activatedCallback = [=](Item &) -> bool {
        application->returnToPreviousWindow();
        return true;
    };

    yes = new Label(this,
                    style::yes::x,
                    style::yes::y,
                    style::yes::w,
                    style::yes::h,
                    utils::localize.get(style::strings::common::yes));
    yes->setPenWidth(0);
    yes->setPenFocusWidth(3);
    yes->setFilled(false);
    yes->setBorderColor(ColorFullBlack);
    yes->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    yes->setFont(style::window::font::big);
    yes->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    yes->activatedCallback = [=](Item &) -> bool { return meta.action(); };

    no->setNavigationItem(NavigationDirection::RIGHT, yes);
    yes->setNavigationItem(NavigationDirection::LEFT, no);
    no->setNavigationItem(NavigationDirection::LEFT, yes);
    yes->setNavigationItem(NavigationDirection::RIGHT, no);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_confirm"));

    setFocusItem(no);
}

void DialogYesNo::update(const Meta &meta)
{
    Dialog::update(meta);

    yes->activatedCallback = [=](Item &) -> bool { return meta.action(); };

    setFocusItem(no);
}

DialogYesNoIconTxt::DialogYesNoIconTxt(app::Application *app, const std::string &name, const Meta &meta)
    : DialogYesNo(app, name, meta)
{
    iconText = new Text(this, style::icontext::x, style::icontext::y, style::icontext::w, style::icontext::h);
    iconText->setText(textStr);
    iconText->setTextType(TextType::SINGLE_LINE);
    iconText->setEditMode(EditMode::BROWSE);
    iconText->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    iconText->setFont(style::window::font::largelight);
    iconText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::None));
    setFocusItem(no);
}

void DialogYesNoIconTxt::update(const Meta &meta)
{
    DialogYesNo::update(meta);
    iconText->setText(textStr);
    topBar->setActive(TopBar::Elements::BATTERY, false);
    topBar->setActive(TopBar::Elements::SIM, false);
    setFocusItem(no);
}

void DialogYesNoIconTxt::SetIconText(const std::string &text)
{
    textStr = text;
}
