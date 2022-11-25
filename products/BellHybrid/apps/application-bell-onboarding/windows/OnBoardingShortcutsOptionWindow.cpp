// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingShortcutsOptionWindow.hpp"
#include "BellOnBoardingNames.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <i18n/i18n.hpp>
#include <common/options/OptionBellMenu.hpp>

namespace gui
{
    OnBoardingShortcutsOptionWindow::OnBoardingShortcutsOptionWindow(app::ApplicationCommon *app,
                                                                     const std::string &name)
        : BellShortOptionWindow(app, name), startString{utils::translate("app_bell_onboarding_shortcuts_start")},
          skipString{utils::translate("app_bell_onboarding_shortcuts_skip")}
    {
        body->firstBox->setMinimumHeight(style::bell_shortcuts_layout::top_layout_h);
        body->firstBox->setMaximumHeight(style::bell_shortcuts_layout::top_layout_h);
        body->centerBox->setMinimumSize(style::bell_base_layout::center_layout_w,
                                        style::bell_shortcuts_layout::center_layout_h);
        body->centerBox->setMaximumSize(style::bell_base_layout::center_layout_w,
                                        style::bell_shortcuts_layout::center_layout_h);
        body->lastBox->setMinimumHeight(style::bell_shortcuts_layout::bottom_layout_h);
        body->lastBox->setMaximumHeight(style::bell_shortcuts_layout::bottom_layout_h);
        optionsList->setAlignment(Alignment(Alignment::Vertical::Center));
        optionsList->setMinimumSize(style::bell_options::default_text_width,
                                    style::bell_shortcuts_layout::center_layout_h);
        optionsList->setMaximumSize(style::bell_options::default_text_width,
                                    style::bell_shortcuts_layout::center_layout_h);

        addOptions(settingsOptionsList());
        setListTitle(utils::translate("app_bell_onboarding_shortcuts_question"),
                     style::bell_shortcuts_layout::top_layout_h);

        body->resize();
        body->resizeItems();
    }

    std::list<Option> OnBoardingShortcutsOptionWindow::settingsOptionsList()
    {
        std::list<gui::Option> settingsOptionList;
        auto addWinSettings = [&](const UTF8 &name, std::function<bool(Item &)> activatedCallback) {
            settingsOptionList.emplace_back(std::make_unique<gui::option::OptionBellMenu>(
                name, activatedCallback, [=](gui::Item &) { return true; }, this));
        };

        addWinSettings(startString, [this](auto &) {
            getApplication()->switchWindow(gui::window::name::onBoardingShortcutsWindow);
            return true;
        });
        addWinSettings(skipString, [this](auto &) {
            getApplication()->switchWindow(gui::window::name::onBoardingSettingsWindow);
            return true;
        });

        return settingsOptionList;
    }
} /* namespace gui */
