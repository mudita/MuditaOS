// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmHandler.hpp"
#include "src/actions/PlayAudioActions.hpp"
#include "src/actions/NotifyGUIAction.hpp"
#include "src/actions/FrontlightAction.hpp"
#include "src/actions/NotifyGUIBedtimeReminderAction.hpp"

namespace alarms
{
    BellAlarmHandler::BellAlarmHandler(Actions &&actions) : actions{std::move(actions)}
    {}

    bool BellAlarmHandler::handle(const AlarmEventRecord &record)
    {
        auto result{true};
        if (record.enabled) {
            for (const auto &action : actions) {
                result &= action->execute();
            }
        }
        return result;
    }

    bool BellAlarmHandler::handleOff([[maybe_unused]] const AlarmEventRecord &record)
    {
        auto result{true};
        for (const auto &action : actions) {
            result &= action->turnOff();
        }
        return result;
    }

    BellAlarmClockHandler::BellAlarmClockHandler(sys::Service *service) : BellAlarmHandler{getActions(service)}
    {}

    auto BellAlarmClockHandler::getActions(sys::Service *service) -> Actions
    {
        Actions actions;
        actions.emplace_back(factory::createAlarmToneAction(*service));
        actions.emplace_back(std::make_unique<NotifyGUIAction>(*service));
        actions.emplace_back(std::make_unique<FrontlightAction>(
            *service, FrontlightAction::Mode::Manual, FrontlightAction::SettingsDependency::AlarmClock));
        return actions;
    }

    PreWakeUpChimeHandler::PreWakeUpChimeHandler(sys::Service *service) : BellAlarmHandler{getActions(service)}
    {}

    auto PreWakeUpChimeHandler::getActions(sys::Service *service) -> Actions
    {
        Actions actions;
        actions.emplace_back(factory::createPreWakeUpChimeAction(*service));
        return actions;
    }

    PreWakeUpFrontlightHandler::PreWakeUpFrontlightHandler(sys::Service *service)
        : BellAlarmHandler(getActions(service))
    {}

    auto PreWakeUpFrontlightHandler::getActions(sys::Service *service) -> Actions
    {
        Actions actions;
        actions.emplace_back(std::make_unique<FrontlightAction>(
            *service, FrontlightAction::Mode::LinearProgress, FrontlightAction::SettingsDependency::Prewakeup));
        return actions;
    }

    SnoozeChimeHandler::SnoozeChimeHandler(sys::Service *service) : BellAlarmHandler{getActions(service)}
    {}

    auto SnoozeChimeHandler::getActions(sys::Service *service) -> Actions
    {
        Actions actions;
        actions.emplace_back(factory::createSnoozeChimeAction(*service));
        return actions;
    }

    BedtimeReminderHandler::BedtimeReminderHandler(sys::Service *service) : BellAlarmHandler{getActions(service)}
    {}

    auto BedtimeReminderHandler::getActions(sys::Service *service) -> Actions
    {
        Actions actions;
        actions.emplace_back(factory::createBedtimeChimeAction(*service));
        actions.emplace_back(std::make_unique<NotifyGUIBedtimeReminderAction>(*service));
        return actions;
    }
} // namespace alarms
