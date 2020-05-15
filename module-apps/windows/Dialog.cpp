#include "Dialog.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <i18/i18.hpp>

using namespace gui;

namespace style
{
    namespace image
    {
        constexpr uint32_t x = 176;
        constexpr uint32_t y = 135;
    } // namespace image
    namespace text
    {
        constexpr uint32_t x = 45;
        constexpr uint32_t y = 293;
        constexpr uint32_t w = 400;
        constexpr uint32_t h = 66;
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

} // namespace style

Dialog::Dialog(app::Application *app, const std::string &name, const Dialog::Meta &meta)
    : gui::AppWindow(app, name), meta(meta)
{
    AppWindow::buildInterface();
    // TODO fix elements positioning with styles ready, right now copied from Phonebook as it is

    topBar->setActive(TopBar::Elements::TIME, true);
    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    setTitle(meta.title);

    icon = new Image(this, style::image::x, style::image::y, meta.icon);

    text = new Text(this, style::text::x, style::text::y, style::text::w, style::text::h);
    text->setText(meta.text);
    text->setTextType(Text::TextType::MULTI_LINE);
    text->setEditMode(Text::EditMode::BROWSE);
    text->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    text->setFont(style::window::font::medium);
    text->setAlignment(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_CENTER, gui::Alignment::ALIGN_VERTICAL_CENTER));
}

bool Dialog::onInput(const InputEvent &inputEvent)
{
    return AppWindow::onInput(inputEvent);
}

void Dialog::buildInterface()
{}

void Dialog::update(const Meta &meta)
{
    this->meta = meta;
    setTitle(meta.title);
    text->setText(meta.text);
    if (meta.have_choice) {
        setupChoice();
    }
}

void Dialog::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(no);
}

void Dialog::setupChoice()
{
    erase(no);
    no = new Label(
        this, style::no::x, style::no::y, style::no::w, style::no::h, utils::localize.get(style::strings::common::no));
    no->setPenWidth(0);
    no->setPenFocusWidth(3);
    no->setFilled(false);
    no->setBorderColor(ColorFullBlack);
    no->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    no->setFont(style::window::font::big);
    no->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    no->activatedCallback = [=](Item &) -> bool { return application->returnToPreviousWindow(); };

    erase(yes);
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
    yes->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    yes->activatedCallback = [=](Item &) -> bool { return meta.action(); };

    no->setNavigationItem(NavigationDirection::RIGHT, yes);
    yes->setNavigationItem(NavigationDirection::LEFT, no);
    no->setNavigationItem(NavigationDirection::LEFT, yes);
    yes->setNavigationItem(NavigationDirection::RIGHT, no);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_confirm"));

    setFocusItem(no);
}
