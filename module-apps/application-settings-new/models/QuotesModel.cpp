// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/windows/QuotesMainWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "QuotesModel.hpp"

#include <InputEvent.hpp>
#include <i18n/i18n.hpp>
#include <json/json11.hpp>
#include <Utils.hpp>
#include <string>
#include <utility>
#include <service-db/QuotesMessages.hpp>

namespace style::quotes::list
{
    constexpr auto item_height = 63;
} // namespace style::quotes::list

namespace Quotes
{
    QuotesModel::QuotesModel(app::Application *application) : app(application)
    {}

    auto QuotesModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto QuotesModel::getMinimalItemHeight() const -> unsigned int
    {
        return style::quotes::list::item_height;
    }

    void QuotesModel::requestRecords(const uint32_t offset, const uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto QuotesModel::getItem(gui::Order order) -> gui::ListItem *
    {
        auto *item = dynamic_cast<gui::QuoteWidget *>(getRecord(order));

        if (item != nullptr) {
            item->inputCallback = [app = app, item](gui::Item &, const gui::InputEvent &event) {
                if (event.isShortPress() && event.is(gui::KeyCode::KEY_LF)) {
                    app->switchWindow(
                        gui::window::name::options_quote,
                        std::make_unique<gui::QuoteSwitchData>(gui::QuoteAction::None, item->getQuoteData()));
                }
                return false;
            };
        }
        return item;
    }

    void QuotesModel::rebuild()
    {
        list->clear();
        eraseInternalData();
        createData();
        list->rebuildList();
    }

    void QuotesModel::createData()
    {
        const auto customCategoryId = getCustomCategoryId();
        if (!customCategoryId) {
            LOG_WARN("No CUSTOM category in database!");
            return;
        }

        auto quotesList    = std::make_unique<QuotesList>();
        quotesList->limit  = 0;
        quotesList->offset = 0;

        auto request =
            std::make_shared<Messages::GetQuotesListByCategoryIdRequest>(std::move(quotesList), *customCategoryId);
        auto result = app->bus.sendUnicast(request, service::name::db, DBServiceAPI::DefaultTimeoutInMs);

        if (result.first != sys::ReturnCodes::Success) {
            LOG_WARN("Getting quotes list by category id failed! error code = %d", static_cast<int>(result.first));
            return;
        }

        auto response = std::dynamic_pointer_cast<Messages::GetQuotesListByCategoryIdResponse>(result.second);
        if (response) {
            LOG_WARN("Wrong response on quotes list by category id request!");
            return;
        }
        LOG_DEBUG("Quotes list count: %u", response->getCount());

        for (const auto &quote : response->getResults()) {
            auto item = new gui::QuoteWidget(
                quote,
                [app = app, quote](bool enable) -> bool {
                    LOG_DEBUG("Sending enable quote request: quote_id = %d, enable = %d", quote.quote_id, enable);
                    auto request = std::make_shared<Messages::EnableQuoteByIdRequest>(quote.quote_id, enable);
                    auto result  = app->bus.sendUnicast(request, service::name::db, DBServiceAPI::DefaultTimeoutInMs);

                    if (result.first == sys::ReturnCodes::Success) {
                        auto response = std::dynamic_pointer_cast<Messages::EnableQuoteByIdResponse>(result.second);
                        const auto success = response && response->success;
                        if (!success)
                            LOG_WARN("Enable quote request failed!");
                        return success;
                    }

                    LOG_WARN("Enable quote request failed! error code = %d", static_cast<int>(result.first));
                    return false;
                },
                [app = app](const UTF8 &text) {
                    app->getCurrentWindow()->bottomBarTemporaryMode(text, gui::BottomBar::Side::CENTER, false);
                },
                [app = app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

            item->deleteByList = false;
            internalData.push_back(item);
        }
    }

    void QuotesModel::remove(const Quotes::QuoteRecord &quote)
    {
    }

    void QuotesModel::save(const Quotes::QuoteRecord &quote)
    {}

    auto QuotesModel::getCustomCategoryId() -> std::optional<unsigned int>
    {
        auto categoryList    = std::make_unique<CategoryList>();
        categoryList->limit  = 0;
        categoryList->offset = 0;

        auto request = std::make_shared<Messages::GetCategoryListRequest>(std::move(categoryList));
        auto result  = app->bus.sendUnicast(request, service::name::db, DBServiceAPI::DefaultTimeoutInMs);

        if (result.first != sys::ReturnCodes::Success) {
            LOG_WARN("Getting category list failed! error code = %d", static_cast<int>(result.first));
            return std::nullopt;
        }

        auto response = std::dynamic_pointer_cast<Messages::GetCategoryListResponse>(result.second);
        if (response) {
            LOG_WARN("Wrong response on category list request!");
            return std::nullopt;
        }

        const auto categories = response->getResults();
        const auto it         = std::find_if(categories.begin(), categories.end(), [](const CategoryRecord &rec) {
            return strcmp(rec.category_name.c_str(), customCategoryName) == 0;
        });
        return (it != categories.end()) ? std::optional<unsigned int>(it->category_id) : std::nullopt;
    }

} // namespace Quotes
