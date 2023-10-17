// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatisticsPresenter.hpp"
#include "widgets/SummaryListItem.hpp"
#include "models/Statistics.hpp"
#include "MeditationMainWindow.hpp"

#include <db/MeditationStatsMessages.hpp>
#include <ApplicationCommon.hpp>
#include <common/BellListItemProvider.hpp>

namespace
{
    std::string createTitle(const std::string &str, const std::uint32_t days)
    {
        auto parser       = gui::text::RichTextParser{};
        const auto result = parser.parse(
            utils::translate(str), nullptr, gui::text::RichTextParser::TokenMap({{"$VALUE", std::to_string(days)}}));
        return result->getText();
    }
} // namespace

namespace app::meditation
{
    StatisticsPresenter::StatisticsPresenter(app::ApplicationCommon *app, const models::Statistics &statisticsModel)
        : app{app}
    {
        BellListItemProvider::Items listItems;
        for (const auto e : std::array<std::uint32_t, 3>{7, 30, 365}) {
            if (const auto summary = statisticsModel.getSummary(e)) {
                auto listItem =
                    new SummaryListItem(createTitle("app_meditation_summary_title", e), summary->sum, summary->avg);
                listItems.push_back(listItem);
            }
            else {
                LOG_ERROR("Fetching summary for the last %" PRIu32 " days failed", e);
            }
        }

        listItemsProvider = std::make_shared<BellListItemProvider>(std::move(listItems));
    }
    void StatisticsPresenter::eraseProviderData()
    {
        listItemsProvider->clearData();
    }
    auto StatisticsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return listItemsProvider;
    }
    void StatisticsPresenter::handleExit()
    {
        app->returnToPreviousWindow();
    }
} // namespace app::meditation
