// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <vector>

#include "AppWindow.hpp"
#include "GridLayout.hpp"
#include "widgets/BottomBar.hpp"
#include "widgets/Label.hpp"
#include "widgets/Rect.hpp"

#include "utf8/UTF8.hpp"

namespace gui
{
    class Image;
    /// icon, description, name of application to run
    struct Tile : public Rect
    {
        Tile(UTF8 icon,
             std::string title,
             std::function<bool(Item &)> activatedCallback,
             std::function<bool()> hasNotificationsCallback = nullptr);

        bool onNotificationsChange(gui::RefreshModes);

      private:
        std::function<bool(gui::RefreshModes)> onNotificationsChangeCallback = nullptr;
        gui::Image *notificationThumbnail                                    = nullptr;
    };

    class MenuPage : public gui::GridLayout
    {
      public:
        bool first_time_selection = true;
        /// position of element which should be selected on start
        const unsigned int first_time_selected = 3;

        UTF8 title;
        MenuPage(gui::Item *parent, UTF8 title, std::vector<Tile *> tiles);
        /// set child which should be selected on start of desktop
        void setFirstTimeSelection();
        bool refresh(gui::RefreshModes mode);
    };

    class MenuWindow : public AppWindow
    {
      protected:
        // page that is currently selected by the user.
        MenuPage *mainMenu  = nullptr;
        MenuPage *toolsMenu = nullptr;

      public:
        MenuWindow(app::Application *app);

        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        void switchMenu(MenuPage *page);
        void refresh();

      private:
        void invalidate() noexcept;
    };

} /* namespace gui */
