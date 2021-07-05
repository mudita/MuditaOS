// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ConnectionFrequencyWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "OptionSetting.hpp"
#include <i18n/i18n.hpp>

namespace gui
{
    ConnectionFrequencyWindow::ConnectionFrequencyWindow(app::Application *app,
                                                         app::settingsInterface::ConnectionSettings *connectionSettings)
        : BaseSettingsWindow(app, gui::window::name::connection_frequency), connectionSettings(connectionSettings)
    {}
    void ConnectionFrequencyWindow::onBeforeShow(ShowMode m, SwitchData *d)
    {
        rebuild();
    }
    auto ConnectionFrequencyWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optList;
        std::vector<uint8_t> frequency{0, 15, 30, 45, 60};

        auto intervalText = [](uint8_t value) {
            if (value == 0) {
                return utils::translate("app_alarm_clock_repeat_never");
            }
            const std::string toReplace = "%0";
            std::string temp            = utils::translate("app_meditation_interval_every_x_minutes");
            temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(value));
            return temp;
        };

        for (auto freq : frequency) {
            optList.emplace_back(std::make_unique<gui::option::OptionSettings>(
                intervalText(freq),
                [=](gui::Item &item) {
                    updateInterval(freq);
                    return true;
                },
                nullptr,
                nullptr,
                (freq == connectionSettings->getConnectionFrequency()) ? option::SettingRightItem::Checked
                                                                       : option::SettingRightItem::Disabled));
        }

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        return optList;
    }
    void ConnectionFrequencyWindow::rebuild()
    {
        clearOptions();
        addOptions(buildOptionsList());
    }
    void ConnectionFrequencyWindow::updateInterval(uint8_t value)
    {
        connectionSettings->setConnectionFrequency(value);
        refreshOptionsList();
    }

} // namespace gui
