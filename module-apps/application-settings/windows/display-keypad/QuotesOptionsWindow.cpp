// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesOptionsWindow.hpp"

#include <application-settings/models/display-keypad/QuotesModel.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <DialogMetadataMessage.hpp>
#include <OptionSetting.hpp>
#include <Text.hpp>

namespace gui
{

    QuotesOptionsWindow::QuotesOptionsWindow(app::ApplicationCommon *app)
        : BaseSettingsWindow(app, gui::window::name::quotes), quotesModel(std::make_shared<Quotes::QuotesModel>(app))
    {
        setTitle(utils::translate("common_options_title"));
    }

    auto QuotesOptionsWindow::buildOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> optionsList;

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_display_wallpaper_quotes_edit"),
            [=](gui::Item &item) {
                application->switchWindow(gui::window::name::new_quote,
                                          std::make_unique<QuoteSwitchData>(QuoteAction::Edit, quote));
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translate(style::strings::common::select), BottomBar::Side::CENTER);
                }
                return true;
            },
            this));

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_display_wallpaper_quotes_delete"),
            [=](gui::Item &item) {
                auto metaData = std::make_unique<gui::DialogMetadataMessage>(
                    gui::DialogMetadata{quote.quote,
                                        "phonebook_contact_delete_trashcan",
                                        utils::translate("app_settings_display_wallpaper_quotes_delete_confirmation"),
                                        "",
                                        [this]() {
                                            auto backToQuotesMainWindow = 2;
                                            quotesModel->remove(quote);
                                            application->returnToPreviousWindow(backToQuotesMainWindow);
                                            return true;
                                        }});
                application->switchWindow(
                    gui::window::name::quotes_dialog_yes_no, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setBottomBarText(utils::translate(style::strings::common::select), BottomBar::Side::CENTER);
                }
                return true;
            },
            this));

        return optionsList;
    } // namespace gui

    void QuotesOptionsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto *quoteSwitchData = dynamic_cast<QuoteSwitchData *>(data);
        if (quoteSwitchData != nullptr) {
            quote = quoteSwitchData->getQuote();
        }

        BaseSettingsWindow::onBeforeShow(mode, data);
    }
} // namespace gui
