// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CategoriesModel.hpp"

#include <application-settings/widgets/display-keypad/CategoryWidget.hpp>

#include <ListView.hpp>
#include <AppWindow.hpp>

namespace style::quotes::list
{
    constexpr auto item_height = 63;
} // namespace style::quotes::list

namespace Quotes
{
    CategoriesModel::CategoriesModel(app::ApplicationCommon *application)
        : DatabaseModel(application), app::AsyncCallbackReceiver{application}, app(application)
    {}

    auto CategoriesModel::requestRecordsCount() -> unsigned int
    {
        return recordsCount;
    }

    bool CategoriesModel::updateRecords(std::vector<CategoryRecord> records)
    {
        if (DatabaseModel::updateRecords(std::move(records))) {
            list->onProviderDataUpdate();
            return true;
        }
        return false;
    }

    auto CategoriesModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::quotes::list::item_height;
    }

    void CategoriesModel::requestRecords(const uint32_t offset, const uint32_t limit)
    {
        LOG_DEBUG(
            "Request categories: offset = %u, limit = %u", static_cast<unsigned>(offset), static_cast<unsigned>(limit));
        auto query = std::make_unique<Messages::GetCategoryListRequest>(offset, limit);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
        task->setCallback([this](auto response) { return handleQueryResponse(response); });
        task->execute(app, this);
    }

    auto CategoriesModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
    {
        auto msgResponse = dynamic_cast<Messages::GetCategoryListResponse *>(queryResult);
        assert(msgResponse != nullptr);

        // If list record count has changed we need to rebuild list.
        if (recordsCount != (msgResponse->getCount())) {
            recordsCount = msgResponse->getCount();
            list->reSendLastRebuildRequest();
            return false;
        }

        LOG_DEBUG("Categories count: %u", static_cast<unsigned>(msgResponse->getCount()));
        auto records = msgResponse->getResults();
        return this->updateRecords(std::move(records));
    }

    auto CategoriesModel::getItem(gui::Order order) -> gui::ListItem *
    {
        auto category = getRecord(order);

        if (!category) {
            return nullptr;
        }

        auto item = new gui::CategoryWidget(
            *category,
            [this, &category = *category](bool enable) {
                LOG_DEBUG("Sending enable category request: category_id = %u, enable = %d",
                          static_cast<unsigned>(category.category_id),
                          enable);

                auto query = std::make_unique<Messages::EnableCategoryByIdRequest>(category.category_id, enable);
                auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
                task->execute(app, this);
            },
            [app = app](const UTF8 &text) {
                app->getCurrentWindow()->navBarTemporaryMode(text, gui::nav_bar::Side::Center, false);
            },
            [app = app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); });

        return item;
    }
} // namespace Quotes
