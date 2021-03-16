// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/windows/QuoteCategoriesWindow.hpp"
#include "QuotesRepository.hpp"
#include "CategoriesModel.hpp"
#include <service-db/QuotesMessages.hpp>

namespace style::quotes::list
{
    constexpr auto item_height = 63;
    constexpr auto max_quotes  = 100;
} // namespace style::quotes::list

namespace Quotes
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
        const auto categoryList = getCategoryList();
        if (!categoryList) {
            return;
        }

        for (const auto &category : *categoryList) {
            auto app  = application;
            auto item = new gui::CategoryWidget(
                category,
                [app, category](bool enable) -> bool {
                    LOG_DEBUG(
                        "Sending enable category request: category_id = %d, enable = %d", category.category_id, enable);
                    auto request = std::make_shared<Messages::EnableCategoryByIdRequest>(category.category_id, enable);
                    auto result  = app->bus.sendUnicast(request, service::name::db, DBServiceAPI::DefaultTimeoutInMs);

                    if (result.first != sys::ReturnCodes::Success) {
                        LOG_WARN("Enable category request failed! error code = %d", static_cast<int>(result.first));
                        return false;
                    }

                    auto response      = std::dynamic_pointer_cast<Messages::EnableCategoryByIdResponse>(result.second);
                    const auto success = response && response->success;
                    if (!success)
                        LOG_WARN("Enable category request failed!");
                    return success;
                },
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

    auto CategoriesModel::getCategoryList() -> std::optional<std::vector<CategoryRecord>>
    {
        auto categoryList    = std::make_unique<CategoryList>();
        categoryList->limit  = 0;
        categoryList->offset = 0;

        auto request = std::make_shared<Messages::GetCategoryListRequest>(std::move(categoryList));
        auto result =
            application->bus.sendUnicast(std::move(request), service::name::db, DBServiceAPI::DefaultTimeoutInMs);
        if (result.first != sys::ReturnCodes::Success) {
            LOG_WARN("Getting category list failed! error code = %d", static_cast<int>(result.first));
            return std::nullopt;
        }

        auto response = std::dynamic_pointer_cast<Messages::GetCategoryListResponse>(result.second);
        if (!response) {
            LOG_WARN("Wrong response on category list request!");
            return std::nullopt;
        }

        LOG_DEBUG("Categories list count: %u", response->getCount());
        return response->getResults();
    }
} // namespace Quotes
