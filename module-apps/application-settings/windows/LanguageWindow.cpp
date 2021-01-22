// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <functional>

#include "service-appmgr/Controller.hpp"

#include "../ApplicationSettings.hpp"

#include <i18n/i18n.hpp>

#include "Label.hpp"
#include "LanguageWindow.hpp"
#include <Style.hpp>
#include <module-apps/application-settings-new/widgets/SettingsStyle.hpp>

namespace gui
{
    namespace window::name
    {
        constexpr inline auto languages = "Languages";
    }

    LanguageWindow::LanguageWindow(app::Application *app) : AppWindow(app, gui::window::name::languages)
    {
        buildInterface();
        setFocusItem(options[0]);
    }

    void LanguageWindow::rebuild()
    {
        // find which widget has focus
        uint32_t index = 0;
        for (uint32_t i = 0; i < options.size(); i++)
            if (options[i] == getFocusItem()) {
                index = i;
                break;
            }

        destroyInterface();
        buildInterface();
        setFocusItem(options[index]);
    }
    void LanguageWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        setTitle(utils::localize.get("app_settings_title_languages"));
        const auto &langList = loader.getAvailableDisplayLanguages();
        for (const auto &lang : langList) {
            options.push_back(addOptionLabel(lang, [=](gui::Item &item) {
                app::manager::Controller::changeDisplayLanguage(application, lang);
                return true;
            }));
        }

        // set position and navigation for labels
        uint32_t size = options.size();
        for (uint32_t i = 0; i < size; i++) {
            options[i]->setPosition(style::settings::window::languageChange::options_posX,
                                    style::settings::window::languageChange::options_posY +
                                        (i * style::settings::window::languageChange::options_distance_between));
            options[i]->setNavigationItem(NavigationDirection::DOWN, options[(i + 1) % size]);
            options[i]->setNavigationItem(NavigationDirection::UP, options[(size + i - 1) % size]);
        }
    }

    void LanguageWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void LanguageWindow::invalidate() noexcept
    {
        options.clear();
    }

    gui::Item *LanguageWindow::addOptionLabel(const std::string &text, std::function<bool(Item &)> activatedCallback)
    {
        gui::Label *label = new gui::Label(this, 17, 0, 480 - 34, 60, text);
        label->setPadding(gui::Padding(16, 0, 0, 0));
        label->setFilled(false);
        label->setPenFocusWidth(3);
        label->setPenWidth(0);
        label->setFont(style::window::font::medium);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        label->setRadius(11);
        label->activatedCallback = activatedCallback;

        return label;
    }

    void LanguageWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        setFocusItem(options[0]);
    }

} /* namespace gui */
