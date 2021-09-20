// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmHandler.hpp"
#include "src/actions/PlayAudioActions.hpp"
#include "src/actions/NotifyGUIAction.hpp"
#include "src/actions/FrontlightAction.hpp"

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
    {
    }

    auto BellAlarmClockHandler::getActions(sys::Service *service) -> Actions
    {
        Actions actions;
        actions.emplace_back(std::make_unique<PlayToneAction>(*service));
        actions.emplace_back(std::make_unique<NotifyGUIAction>(*service));
        actions.emplace_back(std::make_unique<FrontlightAction>());
        return actions;
    }

    PreWakeUpChimeHandler::PreWakeUpChimeHandler(sys::Service *service) : BellAlarmHandler{getActions(service)}
    {}

    auto PreWakeUpChimeHandler::getActions(sys::Service *service) -> Actions
    {
        Actions actions;
        actions.emplace_back(std::make_unique<PlayChimeAction>(*service));
        return actions;
    }
} // namespace alarms
