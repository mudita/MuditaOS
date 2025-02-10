// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "ShuffleQuoteModel.hpp"

#include "QuotesQueries.hpp"
#include <log/log.hpp>
#include <bsp/trng/trng.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <time/time_constants.hpp>

#include <memory>
#include <ctime>

namespace
{
    using namespace std::chrono;

    constexpr auto zeroOffset = 0;
    constexpr auto maxLimit   = std::numeric_limits<unsigned>::max();
    constexpr auto dayInSec   = duration_cast<seconds>(24h).count();

    bool hasCrossedMidnight(std::time_t current, std::time_t last)
    {
        const auto lastTime    = *std::localtime(&last);
        const auto currentTime = *std::localtime(&current);
        if (currentTime.tm_mday != lastTime.tm_mday) {
            return true;
        }
        return (std::abs(current - last) > dayInSec); // If the day is the same but different month/year
    }

    bool isTimeElapsed(std::time_t current, std::time_t last, std::uint32_t intervalInMinutes)
    {
        const auto elapsedMinutes = std::abs(current - last) / utils::time::secondsInMinute;
        return elapsedMinutes >= intervalInMinutes;
    }
} // namespace

namespace Quotes
{
    ShuffleQuoteModel::ShuffleQuoteModel(settings::Settings &settings, Database *quotesDB)
        : settings(settings), quotesDB(quotesDB),
          serializer(std::make_unique<QuotesSettingsSerializer>()), rngEngine{std::make_unique<std::mt19937>(
                                                                        bsp::trng::getRandomValue())}
    {}

    void ShuffleQuoteModel::updateList(ListUpdateMode updateMode)
    {
        const auto quotesGroup = settings.getValue(settings::Quotes::selectedGroup, settings::SettingsScope::Global);
        auto queryResult       = quotesGroup == customGroup
                                     ? quotesDB->query(Queries::getCustomQuotes)
                                     : quotesDB->query(Queries::getPredefinedQuotes, utils::getDisplayLanguage().c_str());
        auto quotesList        = getList<IdsList, IdRecord>(zeroOffset, maxLimit, std::move(queryResult));
        populateList(std::move(quotesList), updateMode);
    }

    void ShuffleQuoteModel::populateList(std::unique_ptr<IdsList> idsList, ListUpdateMode updateMode)
    {
        IdList list;
        for (const auto &item : idsList->data) {
            list.push_back({QuoteType::Predefined, item.quote_id});
        }

        std::shuffle(std::begin(list), std::end(list), *rngEngine);
        if (updateMode == ListUpdateMode::Normal) {
            auto settingsList = settings.getValue(settings::Quotes::randomQuotesList, settings::SettingsScope::Global);
            if (not settingsList.empty()) {
                return;
            }
        }
        settings.setValue(
            settings::Quotes::randomQuotesList, serializer->serialize(list), settings::SettingsScope::Global);
        LOG_DEBUG("*** ID queue length: %zu", list.size());
    }

    auto ShuffleQuoteModel::getId() -> int
    {
        if (isQuoteExpired()) {
            shiftIdList();
        }

        auto list = serializer->deserialize(
            settings.getValue(settings::Quotes::randomQuotesList, settings::SettingsScope::Global));
        const auto [_, id] = list.front();
        return id;
    }

    void ShuffleQuoteModel::shiftIdList()
    {
        auto list = serializer->deserialize(
            settings.getValue(settings::Quotes::randomQuotesList, settings::SettingsScope::Global));

        if (!list.empty()) {
            list.pop_front();
        }

        if (list.empty()) {
            updateList(ListUpdateMode::Forced);
        }
        else {
            settings.setValue(
                settings::Quotes::randomQuotesList, serializer->serialize(list), settings::SettingsScope::Global);
        }
    }

    auto ShuffleQuoteModel::isQuoteExpired() -> bool
    {
        std::time_t lastTimestamp;
        const auto lastTimestampString =
            settings.getValue(settings::Quotes::randomQuoteIDUpdateTime, settings::SettingsScope::Global);
        const auto interval = settings.getValue(settings::Quotes::selectedInterval, settings::SettingsScope::Global);

        try {
            lastTimestamp = std::stoll(lastTimestampString);
        }
        catch (const std::exception &e) {
            LOG_ERROR("Invalid timestamp, set to 0! Cause: %s", e.what());
            lastTimestamp = 0;
        }

        const auto currentTimestamp = std::time(nullptr);

        if (utils::is_number(interval)) {
            if (isTimeElapsed(currentTimestamp, lastTimestamp, utils::toNumeric(interval))) {
                settings.setValue(settings::Quotes::randomQuoteIDUpdateTime,
                                  utils::to_string(currentTimestamp),
                                  settings::SettingsScope::Global);
                return true;
            }
        }
        else if (interval == atMidnight) {
            if (hasCrossedMidnight(currentTimestamp, lastTimestamp)) {
                settings.setValue(settings::Quotes::randomQuoteIDUpdateTime,
                                  utils::to_string(currentTimestamp),
                                  settings::SettingsScope::Global);
                return true;
            }
        }

        return false;
    }
} // namespace Quotes
