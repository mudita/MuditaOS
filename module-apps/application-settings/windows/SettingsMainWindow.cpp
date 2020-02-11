#include "SettingsMainWindow.hpp"
#include "Info.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"

std::list<gui::Option> mainWindowOptions(app::Application *app)
{
    return {gui::Option{"Info",
             [=](gui::Item &item) {
                 LOG_INFO("switching to TEST UI page");
                 app->switchWindow(gui::window::hw_info, nullptr);
                 return true;
             }},
            {"UI TEST",
             [=](gui::Item &item) {
                 LOG_INFO("switching to TEST UI page");
                 app->switchWindow("TEST_UI", nullptr);
                 return true;
             }},

            {utils::localize.get("app_settings_bt"),
             [=](gui::Item &item) {
                 LOG_INFO("switching to bluetooth page");
                 app->switchWindow("Bluetooth", nullptr);
                 return true;
             }},

            {utils::localize.get("app_settings_language"),
             [=](gui::Item &) {
                 LOG_INFO("switching to language page");
                 app->switchWindow("Languages");
                 return true;
             }},

            {"[None] " + utils::localize.get("app_settings_date_and_time"), [=](gui::Item &) { return true; }},
            {"[None] " + utils::localize.get("app_settings_display"), [=](gui::Item &) { return true; }},
            {"[None] " + utils::localize.get("app_settings_phone_modes"), [=](gui::Item &) { return true; }},
            {"[None] " + utils::localize.get("app_settings_security"), [=](gui::Item &) { return true; }},
            {"[None] " + utils::localize.get("app_settings_about"), [=](gui::Item &) { return true; }},
            {"[Dummy]", [=](gui::Item &) { return true; }},
            {"[Dummy]", [=](gui::Item &) { return true; }},
            {"[Dummy]", [=](gui::Item &) { return true; }},
            {"[Dummy]", [=](gui::Item &) { return true; }},
            {"[Dummy]", [=](gui::Item &) { return true; }}};
}
