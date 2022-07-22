// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatisticsPresenter.hpp"
#include "widgets/SummaryListItem.hpp"
#include "models/Statistics.hpp"
#include "MeditationMainWindow.hpp"

#include <db/MeditationStatsMessages.hpp>
#include <ApplicationCommon.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <common/BellListItemProvider.hpp>

namespace app::meditation
{
    StatisticsPresenter::StatisticsPresenter(app::ApplicationCommon *app)
    {
        const auto model = std::make_unique<models::Statistics>(app);

        const auto t1      = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());
        const auto summary = model->getSummary(500);
        const auto t2      = cpp_freertos::Ticks::TicksToMs(cpp_freertos::Ticks::GetTicks());

        auto entry1 = new SummaryListItem("Total [min]", std::to_string(summary->sum.count()));
        auto entry2 = new SummaryListItem("Avg [min]", std::to_string(summary->avg.count()));
        auto entry3 = new SummaryListItem("Entries", std::to_string(summary->count));
        auto entry4 = new SummaryListItem("Query took [ms]", std::to_string(t2 - t1));

        listItemsProvider = std::make_shared<BellListItemProvider>(
            BellListItemProvider::Items{reinterpret_cast<gui::BellSideListItemWithCallbacks *>(entry1),
                                        reinterpret_cast<gui::BellSideListItemWithCallbacks *>(entry2),
                                        reinterpret_cast<gui::BellSideListItemWithCallbacks *>(entry3),
                                        reinterpret_cast<gui::BellSideListItemWithCallbacks *>(entry4)});
    }
    void StatisticsPresenter::eraseProviderData()
    {
        listItemsProvider->clearData();
    }
    void StatisticsPresenter::loadData()
    {}
    void StatisticsPresenter::saveData()
    {}
    auto StatisticsPresenter::getPagesProvider() const -> std::shared_ptr<gui::ListItemProvider>
    {
        return listItemsProvider;
    }
    void StatisticsPresenter::handleEnter()
    {
        app->switchWindow(
            gui::window::bell_finished::defaultName,
            gui::BellFinishedWindowData::Factory::create("circle_success_big", MeditationMainWindow::defaultName));
    }
    void StatisticsPresenter::exitWithoutSave()
    {}
} // namespace app::meditation
