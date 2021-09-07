// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmHandler.hpp"
#include "src/actions/PlayToneAction.hpp"
#include "src/actions/NotifyGUIAction.hpp"
#include "src/actions/FrontlightAction.hpp"

namespace alarms
{
    BellAlarmClockHandler::BellAlarmClockHandler(sys::Service *service) : service{service}
    {
        actions.emplace_back(std::make_unique<PlayToneAction>(*service));
        actions.emplace_back(std::make_unique<NotifyGUIAction>(*service));
        actions.emplace_back(std::make_unique<FrontlightAction>());
    }

    auto BellAlarmClockHandler::handle(const AlarmEventRecord &record) -> bool
    {
        LOG_DEBUG("BellAlarmClockHandler");

        auto result{true};

        if (record.enabled) {
            for (const auto &action : actions) {
                result &= action->execute();
            }
        }

        return result;
    }

    auto EveningReminderHandler::handle(const AlarmEventRecord &record) -> bool
    {
        LOG_DEBUG("EveningReminderHandler");
        // implement this alarm type handling here
        return true;
    }

} // namespace alarms
