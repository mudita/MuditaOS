// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesModel.hpp"

#include <application-settings/widgets/display-keypad/QuoteWidget.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <InputEvent.hpp>
#include <ListView.hpp>

namespace style::quotes::list
{
    constexpr auto item_height = 63;
} // namespace style::quotes::list

namespace Quotes
{
    QuotesModel::QuotesModel(app::ApplicationCommon *application)
        : DatabaseModel(application), app::AsyncCallbackReceiver{application}, app(application)
    {}

    auto QuotesModel::requestRecordsCount() -> unsigned int
    {
        return recordsCount;
    }

    bool QuotesModel::updateRecords(std::vector<QuoteRecord> records)
    {
        if (DatabaseModel::updateRecords(std::move(records))) {
            list->onProviderDataUpdate();
            return true;
        }
        return false;
    }

    auto QuotesModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::quotes::list::item_height;
    }

    void QuotesModel::requestRecords(const uint32_t offset, const uint32_t limit)
    {
        LOG_DEBUG(
            "Request quotes: offset = %u, limit = %u", static_cast<unsigned>(offset), static_cast<unsigned>(limit));
        auto query = std::make_unique<Messages::GetQuotesListFromCustomCategoryRequest>(offset, limit);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
        task->setCallback([this](auto response) { return handleQueryResponse(response); });
        task->execute(app, this);
    }

    auto QuotesModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
    {
        auto msgResponse = dynamic_cast<Messages::GetQuotesListFromCustomCategoryResponse *>(queryResult);
        assert(msgResponse != nullptr);

        // If list record count has changed we need to rebuild list.
        if (recordsCount != (msgResponse->getCount())) {
            recordsCount = msgResponse->getCount();
            list->reSendLastRebuildRequest();
            return false;
        }

        LOG_DEBUG("Quotes count: %u", static_cast<unsigned>(msgResponse->getCount()));
        auto records = msgResponse->getResults();
        return this->updateRecords(std::move(records));
    }

    auto QuotesModel::getItem(gui::Order order) -> gui::ListItem *
    {
        auto quote = getRecord(order);

        if (!quote) {
            return nullptr;
        }

        auto item = new gui::QuoteWidget(
            *quote,
            [this, &quote = *quote](bool enable) {
                LOG_DEBUG("Sending enable quote request: quote_id = %u, enable = %d",
                          static_cast<unsigned>(quote.quote_id),
                          enable);

                auto query = std::make_unique<Messages::EnableQuoteByIdRequest>(quote.quote_id, enable);
                auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
                task->execute(app, this);
            },
            [app = app](const UTF8 &text) {
                app->getCurrentWindow()->bottomBarTemporaryMode(text, gui::BottomBar::Side::CENTER, false);
            },
            [app = app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

        item->inputCallback = [app = app, item](gui::Item &, const gui::InputEvent &event) {
            if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                app->switchWindow(gui::window::name::options_quote,
                                  std::make_unique<gui::QuoteSwitchData>(gui::QuoteAction::None, item->getQuoteData()));
            }
            return false;
        };

        return item;
    }

    void QuotesModel::add(const Quotes::QuoteRecord &record)
    {
        LOG_DEBUG("Adding quote: lang_id = %u", static_cast<unsigned>(record.lang_id));
        auto query = std::make_unique<Messages::AddQuoteRequest>(record.lang_id, record.quote, record.author, true);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
        task->execute(app, this);
    }

    void QuotesModel::edit(const Quotes::QuoteRecord &record)
    {
        LOG_DEBUG("Saving quote: lang_id = %u", static_cast<unsigned>(record.lang_id));

        auto query = std::make_unique<Messages::WriteQuoteRequest>(
            record.quote_id, record.lang_id, record.quote, record.author, record.enabled);
        auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
        task->execute(app, this);
    }

    void QuotesModel::remove(const Quotes::QuoteRecord &record)
    {
        LOG_DEBUG("Removing quote: lang_id = %u", static_cast<unsigned>(record.lang_id));

        auto query = std::make_unique<Messages::DeleteQuoteRequest>(record.quote_id);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Quotes);
        task->execute(app, this);
    }

} // namespace Quotes
