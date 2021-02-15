#include "ChangeTimeZone.hpp"

#include <application-settings-new/ApplicationSettings.hpp>

namespace gui
{

    ChangeTimeZone::ChangeTimeZone(app::Application *app) : BaseSettingsWindow(app, window::name::change_date_and_time)
    {
        setTitle(utils::localize.get("app_settings_date_and_time_time_zone"));
    }

    std::list<Option> ChangeTimeZone::buildOptionsList()
    {
        return {};
    }

} // namespace gui
