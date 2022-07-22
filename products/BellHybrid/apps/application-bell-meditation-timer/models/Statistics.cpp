// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Statistics.hpp"
#include <ApplicationCommon.hpp>
#include <db/ServiceDB.hpp>
#include <db/MeditationStatsMessages.hpp>

namespace
{
    using namespace service::db::meditation_stats;
    std::optional<messages::Response> sendDBRequest(sys::Service *serv, std::shared_ptr<sys::Message> &&msg)
    {
        const auto ret = serv->bus.sendUnicastSync(std::move(msg), service::name::db, sys::BusProxy::defaultTimeout);
        if (ret.first == sys::ReturnCodes::Success) {
            if (auto resp = std::dynamic_pointer_cast<messages::Response>(ret.second)) {
                return *resp;
            }
        }
        return std::nullopt;
    }
} // namespace

namespace app::meditation::models
{

    Statistics::Statistics(app::ApplicationCommon *app) : app::AsyncCallbackReceiver{app}, app{app}
    {}

    void Statistics::addEntry(const time_t utcTimestamp, const std::chrono::minutes duration)
    {
        const auto addRequest = AsyncRequest::createFromMessage(
            std::make_unique<messages::Add>(Entry(utcTimestamp, duration)), service::name::db);
        addRequest->execute(app, this, [this](sys::ResponseMessage *) { return true; });
    }

    std::optional<Summary> Statistics::getSummary(const std::uint32_t days)
    {
        const auto result = sendDBRequest(app, std::make_shared<messages::GetByDays>(days));
        if (not result) {
            return std::nullopt;
        }

        const auto sum   = std::accumulate(result->entries.cbegin(),
                                         result->entries.cend(),
                                         std::chrono::minutes{},
                                         [](const auto &sum, const auto &e) { return sum + e.duration; });
        const auto avg   = std::chrono::minutes{sum / days};
        const auto count = result->entries.size();

        return Summary{sum, avg, count};
    }
} // namespace app::meditation::models