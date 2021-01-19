// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-settings-new/windows/QuotesMainWindow.hpp"
#include "application-settings-new/ApplicationSettings.hpp"
#include "application-settings-new/model/QuotesRepository.hpp"
#include "application-settings-new/model/QuotesModel.hpp"
#include "application-settings-new/model/QuotesModel.hpp"

#include <InputEvent.hpp>
#include <i18n/i18n.hpp>
#include <json/json11.hpp>
#include <Utils.hpp>
#include <string>
#include <utility>

namespace style::quotes::list
{
    constexpr auto item_height = 63;
    constexpr auto max_quotes  = 100;
} // namespace style::quotes::list

namespace app
{
    QuotesModel::QuotesModel(app::Application *app, std::unique_ptr<QuotesRepository> repository)
        : application(app), repository(std::move(repository))
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
        auto app   = application;
        auto *item = dynamic_cast<gui::QuoteWidget *>(getRecord(order));

        if (item != nullptr) {
            item->inputCallback = [app, item](gui::Item &, const gui::InputEvent &event) {
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
        repository->get(0, style::quotes::list::max_quotes, [this](const std::list<QuoteRecord> &quotes, unsigned int) {
            auto app = application;
            for (const auto &quote : quotes) {
                auto item = new gui::QuoteWidget(
                    quote,
                    [app](const UTF8 &text) {
                        app->getCurrentWindow()->bottomBarTemporaryMode(text, gui::BottomBar::Side::CENTER, false);
                    },
                    [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

                item->deleteByList = false;
                internalData.push_back(item);
            }
            return true;
        });
    }

    void QuotesModel::remove(const app::QuoteRecord &quote)
    {
        repository->remove(quote);
    }

    void QuotesModel::save(const app::QuoteRecord &quote)
    {
        repository->save(quote);
    }

} // namespace app
