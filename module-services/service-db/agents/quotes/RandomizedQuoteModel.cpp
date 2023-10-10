// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RandomizedQuoteModel.hpp"
#include "QuotesQueries.hpp"

#include "log/log.hpp"
#include <service-db/agents/settings/SystemSettings.hpp>
#include <memory>
#include <sstream>
#include <bsp/trng/trng.hpp>

namespace Quotes
{
    RandomizedQuoteModel::RandomizedQuoteModel(std::unique_ptr<settings::Settings> settings,
                                               Database *predefinedQuotesDB,
                                               Database *customQuotesDB)
        : settings(std::move(settings)), predefinedQuotesDB(predefinedQuotesDB), customQuotesDB(customQuotesDB),
          serializer(std::make_unique<QuotesSettingsSerializer>()), rngEngine{std::make_unique<std::mt19937>(
                                                                        bsp::trng::getRandomValue())}
    {}

    void RandomizedQuoteModel::randomize(IdList &list)
    {
        std::shuffle(std::begin(list), std::end(list), *rngEngine);
    }

    void RandomizedQuoteModel::updateList(ListUpdateMode updateMode)
    {
        auto queryResult      = customQuotesDB->query(Queries::getEnabledCustomQuotes);
        auto customQuotesList = getList<QuotesList, QuoteRecord>(zeroOffset, maxLimit, std::move(queryResult));

        queryResult =
            predefinedQuotesDB->query(Queries::getEnabledPredefinedQuotes, utils::getDisplayLanguage().c_str());
        auto predefinedQuotesList = getList<QuotesList, QuoteRecord>(zeroOffset, maxLimit, std::move(queryResult));
        populateList(std::move(predefinedQuotesList), std::move(customQuotesList), updateMode);
    }

    void RandomizedQuoteModel::populateList(std::unique_ptr<QuotesList> predefinedQuotesList,
                                            std::unique_ptr<QuotesList> customQuotesList,
                                            ListUpdateMode updateMode)
    {
        IdList list;
        for (const auto &item : predefinedQuotesList->data) {
            list.push_back({QuoteType::Predefined, item.quote_id});
        }
        for (const auto &item : customQuotesList->data) {
            list.push_back({QuoteType::Custom, item.quote_id});
        }

        randomize(list);
        if (updateMode == ListUpdateMode::Normal) {
            auto settingsList = settings->getValue(settings::Quotes::randomQuotesList, settings::SettingsScope::Global);
            if (not settingsList.empty()) {
                return;
            }
        }
        settings->setValue(
            settings::Quotes::randomQuotesList, serializer->serialize(list), settings::SettingsScope::Global);
        LOG_INFO("ID queue length: %zu", list.size());
    }

    auto RandomizedQuoteModel::getId() -> QuoteID
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

        if (!list.empty()) {
            list.pop_front();
        }

        if (list.empty()) {
            updateList(ListUpdateMode::Forced);
        }
        else {
            settings->setValue(
                settings::Quotes::randomQuotesList, serializer->serialize(list), settings::SettingsScope::Global);
        }
        LOG_DEBUG("Selected quote ID: %d, type: %s, remaining quotes to next shuffle: %zu",
                  list.front().second,
                  magic_enum::enum_name(list.front().first).data(),
                  list.size());
    }
    auto RandomizedQuoteModel::isIdExpired() -> bool
    {
        const auto lastTimestampString =
            settings->getValue(settings::Quotes::randomQuoteIDUpdateTime, settings::SettingsScope::Global);
        time_t lastTimestamp;

        try {
            lastTimestamp = stol(lastTimestampString);
        }
        catch (std::invalid_argument &e) {
            LOG_ERROR("Invalid timestamp, zeroing! Cause: %s", e.what());
            lastTimestamp = 0;
        }

        const auto currentTimestamp = std::time(nullptr);
        if ((currentTimestamp - lastTimestamp) >= quotesChangingInterval) {
            lastTimestamp = currentTimestamp;
            settings->setValue(settings::Quotes::randomQuoteIDUpdateTime,
                               ::utils::to_string(lastTimestamp),
                               settings::SettingsScope::Global);
            return true;
        }
        return false;
    }
} // namespace Quotes
