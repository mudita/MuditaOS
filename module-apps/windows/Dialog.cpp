#include "Dialog.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <i18/i18.hpp>

using namespace gui;

Dialog::Dialog(app::Application *app, const std::string &name, const Dialog::Meta &meta)
    : gui::AppWindow(app, name), meta(meta)
{
    AppWindow::buildInterface();
    // TODO fix elements positioning with styles ready, right now copied from Phonebook as it is

    topBar->setActive(TopBar::Elements::TIME, true);
    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_confirm"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    setTitle(meta.title);

    icon = new Image(this, 176, 135, 128, 128, meta.icon);

    text = new Text(this, 45, 293, 390, 128);
    text->setText(meta.text);
    text->setTextType(Text::TextType::MULTI_LINE);
    text->setEditMode(Text::EditMode::BROWSE);
    text->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    text->setFont(style::window::font::small);

    no = new Label(this, 75, 415, 150, 75, utils::localize.get("common_no"));
    no->setPenWidth(0);
    no->setPenFocusWidth(3);
    no->setFilled(false);
    no->setBorderColor(ColorFullBlack);
    no->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    no->setFont(style::window::font::small);
    no->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    no->activatedCallback = [=](Item &) -> bool { return returnToPreviousView(); };

    yes = new Label(this, 255, 415, 150, 75, utils::localize.get("common_yes"));
    yes->setPenWidth(0);
    yes->setPenFocusWidth(3);
    yes->setFilled(false);
    yes->setBorderColor(ColorFullBlack);
    yes->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    yes->setFont(style::window::font::small);
    yes->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

    no->setNavigationItem(NavigationDirection::RIGHT, yes);
    yes->setNavigationItem(NavigationDirection::LEFT, no);
    no->setNavigationItem(NavigationDirection::LEFT, yes);
    yes->setNavigationItem(NavigationDirection::RIGHT, no);
    setFocusItem(no);
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
    yes->activatedCallback = [=](Item &) -> bool { return meta.action(); };
}

void Dialog::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(no);
}
