// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/windows/QuoteCategoriesWindow.hpp"
#include "QuotesRepository.hpp"
#include "CategoriesModel.hpp"

namespace style::quotes::list
{
    constexpr auto item_height = 63;
    constexpr auto max_quotes  = 100;
} // namespace style::quotes::list

namespace app
{
    CategoriesModel::CategoriesModel(app::Application *app) : application(app)
    {}

    auto CategoriesModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto CategoriesModel::getMinimalItemHeight() const -> unsigned int
    {
        return style::quotes::list::item_height;
    }

    void CategoriesModel::requestRecords(const uint32_t offset, const uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto CategoriesModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    void CategoriesModel::createData()
    {
        // fake categories for testing
        // matter to be changed with the final connection of the appropriate agent
        std::vector<app::QuoteCategory> categories{{0, "Category 1"},
                                                   {1, "Category 2"},
                                                   {2, "Category 3"},
                                                   {3, "Category 4"},
                                                   {4, "Category 5"},
                                                   {5, "Category 6"}};
        for (const auto &category : categories) {
            auto app  = application;
            auto item = new gui::CategoryWidget(
                category,
                [app](const UTF8 &text) {
                    app->getCurrentWindow()->bottomBarTemporaryMode(text, gui::BottomBar::Side::CENTER, false);
                },
                [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

            item->deleteByList = false;
            internalData.push_back(item);
        }
    }

    void CategoriesModel::rebuild()
    {
        list->clear();
        eraseInternalData();
        createData();
        list->rebuildList();
    }
} // namespace app
