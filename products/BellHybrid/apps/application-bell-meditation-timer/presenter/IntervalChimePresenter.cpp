// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "IntervalChimePresenter.hpp"
#include "MeditationCommon.hpp"
#include "MeditationStyle.hpp"

#include <service-db/Settings.hpp>

namespace app::meditation
{
    constexpr auto intervalsList = {1, 2, 5, 10, 15, 30};

    IntervalChimePresenter::IntervalChimePresenter(app::ApplicationCommon *app, settings::Settings *settings)
        : app{app}, settings{settings}
    {
        initIntervalOptions();
    }

    std::vector<std::string> IntervalChimePresenter::getIntervals() const
    {
        std::vector<std::string> intervalNames;
        for (auto const &option : intervalOptions) {
            intervalNames.push_back(option.second);
        }
        return intervalNames;
    }

    std::string IntervalChimePresenter::getCurrentInterval() const
    {
        // Always set interval as none
        for (auto const &option : intervalOptions) {
            if (option.first == std::chrono::minutes{0}) {
                return option.second;
            }
        }
        return intervalOptions.at(0).second;
    }

    std::string IntervalChimePresenter::getTimeUnitName(std::string value)
    {
        using namespace app::meditationStyle::mtStyle::list;
        for (auto const &option : intervalOptions) {
            if (option.second == value) {
                if (option.first == std::chrono::minutes{0}) {
                    return "";
                }
                if (option.first == std::chrono::minutes{1}) {
                    return utils::translate(timeUnitSingular);
                }
                return utils::translate(timeUnitPlural);
            }
        }
        return "";
    }

    void IntervalChimePresenter::activate(std::string value)
    {
        for (auto const &option : intervalOptions) {
            if (option.second == value) {
                settings->setValue(
                    intervalDBRecordName, utils::to_string(option.first.count()), settings::SettingsScope::AppLocal);
                app->switchWindow(gui::name::window::readyGoing);
                break;
            }
        }
    }

    void IntervalChimePresenter::initIntervalOptions()
    {
        intervalOptions.push_back({std::chrono::minutes{0}, utils::translate("app_bell_meditation_interval_none")});

        const auto duration =
            utils::getNumericValue<int>(settings->getValue(meditationDBRecordName, settings::SettingsScope::AppLocal));
        if (duration == 1) {
            settings->setValue(intervalDBRecordName, utils::to_string(0), settings::SettingsScope::AppLocal);
            app->switchWindow(gui::name::window::readyGoing);
            return;
        }

        for (const auto &interval : intervalsList) {
            if (interval < duration) {
                intervalOptions.push_back({std::chrono::minutes{interval}, getIntervalString(interval)});
            }
        }
    }

    std::string IntervalChimePresenter::getIntervalString(std::uint32_t value)
    {
        const std::string toReplace = "%0";
        std::string temp            = utils::translate("app_bell_meditation_interval_every_x_minutes");
        temp.replace(temp.find(toReplace), toReplace.size(), std::to_string(value));
        return temp;
    }
} // namespace app::meditation
