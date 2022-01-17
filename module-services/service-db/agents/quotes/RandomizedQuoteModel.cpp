// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RandomizedQuoteModel.hpp"
#include "log/log.hpp"
#include "QuotesQueries.hpp"
#include <service-db/agents/settings/SystemSettings.hpp>
#include <algorithm> // std::random_shuffle
#include <memory>
#include <sstream>

namespace Quotes
{
    constexpr inline auto zeroOffset = 0;
    constexpr inline auto maxLimit   = std::numeric_limits<unsigned int>().max();
    RandomizedQuoteModel::RandomizedQuoteModel(std::unique_ptr<settings::Settings> settings, Database *quotesDB)
        : settings(std::move(settings)), quotesDB(quotesDB), serializer(std::make_unique<QuotesSettingsSerializer>())
    {}

    void RandomizedQuoteModel::randomize(IdList &list)
    {
        srand(time(NULL));
        std::random_shuffle(std::begin(list), std::end(list));
    }
    void RandomizedQuoteModel::updateList(bool forced)
    {
        auto queryResult = quotesDB->query(Queries::getEnabledQuotes);
        auto quotesList  = getList<QuotesList, QuoteRecord>(zeroOffset, maxLimit, std::move(queryResult));
        populateList(std::move(quotesList), forced);
    }
    void RandomizedQuoteModel::populateList(std::unique_ptr<QuotesList> quotesList, bool forcedUpdate)
    {
        IdList list;
        for (const auto &item : quotesList->data) {
            list.push_back(item.quote_id);
        }

        randomize(list);
        if (not forcedUpdate) {
            auto settingsList = settings->getValue(settings::Quotes::randomQuotesList, settings::SettingsScope::Global);
            if (not settingsList.empty()) {
                return;
            }
        }
        settings->setValue(
            settings::Quotes::randomQuotesList, serializer->serialize(list), settings::SettingsScope::Global);
        LOG_ERROR("ID queue length: %zu", list.size());
    }

    auto RandomizedQuoteModel::getId() -> int
    {

        if (isIdExpired()) {
            shiftIdList();
        }

        auto list = serializer->deserialize(
            settings->getValue(settings::Quotes::randomQuotesList, settings::SettingsScope::Global));
        return list.front();
    }

    void RandomizedQuoteModel::shiftIdList()
    {
        auto list = serializer->deserialize(
            settings->getValue(settings::Quotes::randomQuotesList, settings::SettingsScope::Global));
        list.pop_front();
        if (list.empty()) {
            updateList(true);
        }
        settings->setValue(
            settings::Quotes::randomQuotesList, serializer->serialize(list), settings::SettingsScope::Global);
        LOG_DEBUG("Selected quote ID: %d, remaining quotes to next shuffle: %zu", list.front(), list.size());
    }
    auto RandomizedQuoteModel::isIdExpired() -> bool
    {
        auto lastTimestampString =
            settings->getValue(settings::Quotes::randomQuoteIDUpdateTime, settings::SettingsScope::Global);
        long lastTimestamp;

        try {
            lastTimestamp = stol(lastTimestampString);
        }
        catch (std::invalid_argument &e) {
            LOG_ERROR("Invalid timestamp, zeroing! Cause: %s", e.what());
            lastTimestamp = 0;
        }

        auto currentTimestamp = std::time(nullptr);
        if ((currentTimestamp - lastTimestamp) >= quotesChangingInterval) {
            lastTimestamp = currentTimestamp;
            settings->setValue(settings::Quotes::randomQuoteIDUpdateTime,
                               utils::to_string(lastTimestamp),
                               settings::SettingsScope::Global);
            return true;
        }
        return false;
    }
} // namespace Quotes
