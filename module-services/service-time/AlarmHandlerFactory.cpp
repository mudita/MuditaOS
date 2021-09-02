// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <map>
#include <memory>

#include <service-time/AlarmHandler.hpp>
#include <service-time/AlarmHandlerFactory.hpp>

namespace alarms
{
    [[nodiscard]] auto AlarmHandlerFactory::getHandler(const AlarmType type) -> const std::shared_ptr<AlarmHandler>
    {
        if (handlers.count(type) != 0u) {
            return handlers.at(type);
        }
        LOG_ERROR("no such alarm type handler defined: %d", static_cast<int>(type));
        return nullptr;
    }

    auto AlarmHandlerFactory::addHandler(const AlarmType type, const std::shared_ptr<AlarmHandler> handler) -> void
    {
        handlers.insert({type, handler});
    }
} // namespace alarms
