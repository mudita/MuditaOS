// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PureAlarmHandler.hpp"
#include "src/actions/PlayAudioActions.hpp"
#include "src/actions/NotifyGUIAction.hpp"

namespace alarms
{
    PureAlarmHandler::PureAlarmHandler(Actions &&actions) : actions{std::move(actions)}
    {}

    bool PureAlarmHandler::handle(const AlarmEventRecord &record)
    {
        auto result{true};
        if (record.enabled) {
            for (const auto &action : actions) {
                result &= action->execute(record);
            }
        }
        return result;
    }

    bool PureAlarmHandler::handleOff([[maybe_unused]] const AlarmEventRecord &record)
    {
        auto result{true};
        for (const auto &action : actions) {
            result &= action->turnOff(record);
        }
        return result;
    }

    PureAlarmClockHandler::PureAlarmClockHandler(sys::Service *service) : PureAlarmHandler{getActions(service)}
    {}

    auto PureAlarmClockHandler::getActions(sys::Service *service) -> Actions
    {
        Actions actions;
        actions.emplace_back(std::make_unique<PlayToneAction>(*service));
        actions.emplace_back(std::make_unique<NotifyGUIAction>(*service));
        return actions;
    }
} // namespace alarms
