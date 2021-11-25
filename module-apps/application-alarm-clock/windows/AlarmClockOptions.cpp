// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmClockOptions.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include "Option.hpp"
#include "windows/DialogMetadata.hpp"
#include "messages/DialogMetadataMessage.hpp"

namespace app::alarmClock
{
    namespace
    {
        void addOption(const std::string &translationId,
                       std::function<bool(gui::Item &)> onClickCallback,
                       std::list<gui::Option> &options)
        {
            options.emplace_back(utils::translate(translationId), onClickCallback);
        }

        void removeAlarm(const AlarmEventRecord &record,
                         ApplicationCommon *application,
                         AbstractAlarmsRepository &alarmsRepository)
        {
            auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                gui::DialogMetadata{utils::translate("app_alarm_clock_title_main"),
                                    "delete_128px_W_G",
                                    utils::translate("app_alarm_clock_delete_confirmation"),
                                    "",
                                    [record, application, &alarmsRepository] {
                                        alarmsRepository.remove(record, [application](bool) {
                                            application->switchWindow(gui::name::window::main_window);
                                        });
                                        return true;
                                    }});
            application->switchWindow(style::alarmClock::window::name::dialogYesNo, std::move(metaData));
        }
    } // namespace

    std::list<gui::Option> alarmsListOptions(ApplicationCommon *application,
                                             const AlarmEventRecord &record,
                                             AbstractAlarmsRepository &alarmsRepository)
    {
        std::list<gui::Option> options;
        addOption(
            {"app_alarm_clock_options_edit"},
            [application, record](gui::Item &) {
                auto rec  = std::make_unique<AlarmEventRecord>(record);
                auto data = std::make_unique<AlarmRecordData>(std::move(rec));
                data->setDescription(style::alarmClock::editAlarm);
                application->switchWindow(style::alarmClock::window::name::newEditAlarm, std::move(data));
                return true;
            },
            options);
        addOption(
            {"app_alarm_clock_options_delete"},
            [application, record, &alarmsRepository](gui::Item &item) {
                removeAlarm(record, application, alarmsRepository);
                return true;
            },
            options);
        addOption(
            {"app_alarm_clock_options_turn_off_all_alarms"},
            [application, &alarmsRepository](gui::Item &) {
                alarmsRepository.turnOffAll([application](bool) { application->returnToPreviousWindow(); });
                return true;
            },
            options);
        return options;
    }
} // namespace app::alarmClock
