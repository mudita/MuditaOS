#include "MenuWindow.hpp"
#include "../ApplicationDesktop.hpp"
#include "InputEvent.hpp"
#include "Item.hpp"
#include "Navigation.hpp"
#include "service-appmgr/ApplicationManager.hpp"

#include <tools/Common.hpp>
#include <Style.hpp>
#include <cassert>
#include <i18/i18.hpp>

namespace style::design
{
    // all these might not be final - as designs for these didn't look like that

    inline const auto tile_w       = 130;
    inline const auto tile_h       = 130;
    inline const auto tile_margin  = 24 - (64 - 50) / 2; // this is waiting for proper adjust-to-design anyway
    inline const auto tile_text_y  = 50;
    inline const auto notify_dot_x = 80;
    inline const auto notify_dot_y = (64 - 50) / 2;
    inline const auto grid_offset  = 20;
}; // namespace style::design

namespace gui
{
    inline const auto APP_SETTINGS_NEW = "ApplicationSettingsNew";
    Tile::Tile(UTF8 icon, std::string title, std::function<bool(Item &)> activatedCallback, unsigned int notifications)
    {
        setSize(style::design::tile_w, style::design::tile_h);

        auto *it = new gui::Item();
        it->setSize(style::design::tile_w, style::design::tile_h - 2 * style::design::tile_margin);
        it->setPosition(area().x, area().y + style::design::tile_margin);

        auto *img = new gui::Image(icon);
        center(it, img, Axis::X);
        it->addWidget(img);

        auto *desc =
            new gui::Label(it, 0, it->area().h - style::design::tile_text_y, it->area().w, style::design::tile_text_y);
        desc->setPenWidth(style::window::default_border_no_focus_w);
        desc->setFont(style::window::font::verysmall);
        desc->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
        desc->setText(utils::localize.get(title));

        if (notifications > 0) {
            auto thumbnail = new gui::Image("dot_12px_hard_alpha_W_G");
            thumbnail->setPosition(style::design::notify_dot_x, style::design::notify_dot_y);
            it->addWidget(thumbnail);
        }

        this->activatedCallback = activatedCallback;
        this->setPenWidth(style::window::default_border_no_focus_w);
        this->setPenFocusWidth(style::window::default_border_focus_w);
        this->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_TOP | RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        addWidget(it);
    };

    MenuPage::MenuPage(gui::Item *parent, UTF8 title, std::vector<Tile *> tiles) : title(title)
    {
        if (parent) {
            parent->addWidget(this);
        }
        grid.x = style::design::tile_w;
        grid.y = style::design::tile_h;
        for (auto &tile : tiles) {
            addWidget(tile);
        }
    }

    void MenuPage::setFirstTimeSelection()
    {
        if (visible && first_time_selection) {
            if (children.size() > first_time_selected) {
                setFocusItem(*std::next(children.begin(), first_time_selected));
                first_time_selection = false;
            }
        }
    }

    MenuWindow::MenuWindow(app::Application *app) : AppWindow(app, app::window::name::desktop_menu)
    {
        buildInterface();
    }

    void MenuWindow::rebuild()
    {
        focusItem = nullptr;
        destroyInterface();
        buildInterface();
    }

    void MenuWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        auto app = dynamic_cast<app::ApplicationDesktop *>(application);
        assert(app);

        mainMenu = new MenuPage(
            this,
            utils::localize.get("app_desktop_menu_title"),
            {

                new gui::Tile{
                    "menu_tools_W_G",
                    "app_desktop_menu_tools",
                    [=](gui::Item &item) {
                        {
                            switchMenu(toolsMenu);
                            return true;
                        }
                    },
                },
                new gui::Tile("menu_alarm_W_G", "app_desktop_menu_alarm", [=](gui::Item &item) { return true; }),

                new gui::Tile("menu_calendar_W_G",
                              "app_desktop_menu_calendar",
                              [=](gui::Item &item) {
                                  sapm::ApplicationManager::messageSwitchApplication(
                                      application, "ApplicationCalendar", gui::name::window::main_window, nullptr);
                                  return true;
                              }),

                new gui::Tile{"menu_phone_W_G",
                              "app_desktop_menu_phone",
                              [=](gui::Item &item) {
                                  LOG_INFO("Call Log");
                                  sapm::ApplicationManager::messageSwitchApplication(
                                      application, "ApplicationCallLog", gui::name::window::main_window, nullptr);
                                  return true;
                              },
                              app->notifications.notRead.Calls},
                new gui::Tile("menu_contacts_W_G",
                              "app_desktop_menu_contacts",
                              [=](gui::Item &item) {
                                  LOG_INFO("Phonebook");
                                  sapm::ApplicationManager::messageSwitchApplication(
                                      application, "ApplicationPhonebook", gui::name::window::main_window, nullptr);
                                  return true;
                              }),

                new gui::Tile{"menu_messages_W_G",
                              "app_desktop_menu_messages",
                              [=](gui::Item &item) {
                                  LOG_INFO("Messages");
                                  sapm::ApplicationManager::messageSwitchApplication(
                                      application, "ApplicationMessages", gui::name::window::main_window, nullptr);
                                  return true;
                              },
                              app->notifications.notRead.SMS},
                new gui::Tile{"menu_music_player_W_G",
                              "app_desktop_menu_music",
                              [=](gui::Item &item) {
                                  LOG_INFO("Music Player");
                                  sapm::ApplicationManager::messageSwitchApplication(
                                      application, "ApplicationMusicPlayer", gui::name::window::main_window, nullptr);
                                  return true;
                              }},
                new gui::Tile{
                    "menu_meditation_W_G", "app_desktop_menu_meditation", [=](gui::Item &item) { return true; }},
                new gui::Tile{"menu_settings_W_G",
                              "app_desktop_menu_settings_new",
                              [=](gui::Item &item) {
                                  LOG_INFO("page 1 settings");
                                  sapm::ApplicationManager::messageSwitchApplication(
                                      application, APP_SETTINGS_NEW, gui::name::window::main_window, nullptr);
                                  return true;
                              }},
            });

        toolsMenu = new MenuPage(
            this,
            utils::localize.get("app_desktop_tools_title"),
            {
                new gui::Tile{"menu_tools_notes_W_G",
                              "app_desktop_tools_notes",
                              [=](gui::Item &item) {
                                  sapm::ApplicationManager::messageSwitchApplication(
                                      application, "ApplicationNotes", gui::name::window::main_window, nullptr);
                                  return true;
                              }},
                new gui::Tile{
                    "menu_tools_calculator_W_G", "app_desktop_tools_calculator", [=](gui::Item &item) { return true; }},
                new gui::Tile{"menu_tools_recorder_W_G",
                              "app_desktop_tools_antenna",
                              [=](gui::Item &item) {
                                  sapm::ApplicationManager::messageSwitchApplication(
                                      application, "ApplicationAntenna", gui::name::window::main_window, nullptr);
                                  return true;
                              }},
            });

        using namespace style::window;
        mainMenu->setSize(this->area().w - default_left_margin - default_right_margin,
                          bottomBar->area().pos(Axis::Y) - this->title->getOffset(Axis::Y) -
                              style::design::grid_offset);
        mainMenu->setPosition(default_left_margin, title->getOffset(Axis::Y) + style::design::grid_offset);
        mainMenu->setVisible(false);

        toolsMenu->setSize(this->area().w - default_left_margin - default_right_margin,
                           bottomBar->area().pos(Axis::Y) - this->title->getOffset(Axis::Y) -
                               style::design::grid_offset);
        toolsMenu->setPosition(default_left_margin, title->getOffset(Axis::Y) + style::design::grid_offset);
        toolsMenu->setVisible(false);

        switchMenu(mainMenu);
        mainMenu->setFirstTimeSelection();
    }

    void MenuWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void MenuWindow::invalidate() noexcept
    {
        mainMenu  = nullptr;
        toolsMenu = nullptr;
    }

    void MenuWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    bool MenuWindow::onInput(const InputEvent &inputEvent)
    {
        if ((inputEvent.state == InputEvent::State::keyReleasedShort) && (inputEvent.keyCode == KeyCode::KEY_RF) &&
            (toolsMenu->visible)) {
            switchMenu(mainMenu);
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void MenuWindow::switchMenu(MenuPage *page)
    {
        if (toolsMenu != page) {
            toolsMenu->setVisible(false);
        }
        if (mainMenu != page) {
            mainMenu->setVisible(false);
        }

        setTitle(page->title);
        setFocusItem(page);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

} /* namespace gui */
