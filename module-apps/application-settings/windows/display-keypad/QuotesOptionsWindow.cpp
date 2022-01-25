// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesOptionsWindow.hpp"

#include <application-settings/models/display-keypad/QuotesModel.hpp>
#include <application-settings/windows/WindowNames.hpp>

#include <DialogMetadataMessage.hpp>
#include <Dialog.hpp>
#include <OptionSetting.hpp>
#include <Text.hpp>
#include <service-appmgr/Controller.hpp>

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
                    this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
                }
                return true;
            },
            this));

        optionsList.emplace_back(std::make_unique<gui::option::OptionSettings>(
            utils::translate("app_settings_display_wallpaper_quotes_delete"),
            [=](gui::Item &item) {
                auto quoteText = quote.quote;
                auto metaData  = std::make_unique<gui::DialogMetadataMessage>(
                    gui::DialogMetadata{quoteText,
                                        "delete_128px_W_G",
                                        utils::translate("app_settings_display_wallpaper_quotes_delete_confirmation"),
                                        "",
                                        [this, quoteText]() {
                                            quotesModel->remove(quote);
                                            return showDeleteNotification(quoteText);
                                        }});
                application->switchWindow(
                    gui::window::name::quotes_dialog_yes_no, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
                return true;
            },
            [=](gui::Item &item) {
                if (item.focus) {
                    this->setNavBarText(utils::translate(style::strings::common::select), nav_bar::Side::Center);
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

    bool QuotesOptionsWindow::showDeleteNotification(std::string titleText)
    {
        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
            gui::DialogMetadata{titleText,
                                "success_128px_W_G",
                                utils::translate("app_settings_display_wallpaper_quotes_delete_success"),
                                "",
                                [=]() -> bool {
                                    auto data                        = std::make_unique<SwitchData>();
                                    data->ignoreCurrentWindowOnStack = true;
                                    this->application->switchWindow(gui::window::name::quotes, std::move(data));
                                    return true;
                                }});
        metaData->ignoreCurrentWindowOnStack = true;
        application->switchWindow(gui::window::name::dialog_confirm, std::move(metaData));
        return true;
    }
} // namespace gui
