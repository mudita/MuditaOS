#include "NoEvents.hpp"
#include <service-appmgr/ApplicationManager.hpp>
#include <i18/i18.hpp>

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
} // namespace style

NoEvents::NoEvents(app::Application *app, const std::string &name, const NoEvents::Meta &meta)
    : gui::AppWindow(app, name), meta(meta)
{}

void NoEvents::buildInterface()
{
    AppWindow::buildInterface();

    topBar->setActive(gui::TopBar::Elements::TIME, true);
    bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
    bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

    setTitle(meta.title);
    leftArrowImage = new gui::Image(this, style::arrow::x, style::arrow::y, 0, 0, "arrow_left");
    newEventImage  = new gui::Image(this, style::cross::x, style::cross::y, 0, 0, "cross");

    emptyListIcon = new gui::Icon(this,
                                  0,
                                  style::header::height,
                                  style::window_width,
                                  style::window_height - style::header::height - style::footer::height,
                                  meta.icon,
                                  utils::localize.get(meta.text));
}

void NoEvents::update(const Meta &meta)
{
    this->meta = meta;
    rebuild();
}

void NoEvents::rebuild()
{
    AppWindow::destroyInterface();
    erase();
    buildInterface();
}

bool NoEvents::onInput(const gui::InputEvent &inputEvent)
{
    if (AppWindow::onInput(inputEvent)) {
        return true;
    }

    if (!inputEvent.isShortPress()) {
        return false;
    }

    if (inputEvent.keyCode == gui::KeyCode::KEY_LEFT) {
        return meta.action();
    }

    return false;
}
