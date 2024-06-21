// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewMainWindow.hpp"
#include "WhatsNewStyle.hpp"
#include <common/options/OptionBellMenu.hpp>
#include <service-appmgr/Controller.hpp>
#include <product/version.hpp>

namespace app::whatsnew
{
    using namespace gui;

    WhatsNewMainWindow::WhatsNewMainWindow(app::ApplicationCommon *app,
                                           std::unique_ptr<WhatsNewMainContract::Presenter> &&presenter,
                                           const std::string &name)
        : BellOptionWithDescriptionWindow(app, name), presenter{std::move(presenter)}
    {
        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bell_whatsnew_title"), whats_new_style::main_window::list_title_font);
        setListDescription(utils::translate("app_bell_whatsnew_version"),
                           gui::BellOptionWithDescriptionWindow::TokenMap({{"$VERSION", std::string(VERSION)}}),
                           whats_new_style::main_window::description_font,
                           whats_new_style::main_window::description_height);
    }

    auto WhatsNewMainWindow::settingsOptionsList() -> std::list<Option>
    {
        std::list<Option> settingsOptionList;

        auto addWinSettings = [&](const UTF8 &name, const std::function<bool(Item &)> &activatedCallback) {
            settingsOptionList.emplace_back(std::make_unique<option::OptionBellMenu>(
                name, activatedCallback, []([[maybe_unused]] Item &item) { return true; }, this));
        };

        addWinSettings(utils::translate("app_bell_whatsnew_continue"), [this]([[maybe_unused]] Item &item) {
            application->switchWindow(window::name::features);
            return true;
        });
        addWinSettings(utils::translate("app_bell_whatsnew_skip"), [this]([[maybe_unused]] Item &item) {
            skipFeatures();
            return true;
        });

        return settingsOptionList;
    }

    auto WhatsNewMainWindow::onBeforeShow([[maybe_unused]] ShowMode mode, [[maybe_unused]] SwitchData *data) -> void
    {
        static_cast<Application *>(application)->suspendIdleTimer();

        if (presenter->getFeaturesCount() == 0) {
            LOG_INFO("Skipping What's New due to lack of new features");
            skipFeatures();
        }
    }

    auto WhatsNewMainWindow::skipFeatures() -> void
    {
        static_cast<Application *>(application)->resumeIdleTimer();

        presenter->setCurrentOsVersion();
        app::manager::Controller::sendAction(application, app::manager::actions::Home);
    }
} // namespace app::whatsnew
