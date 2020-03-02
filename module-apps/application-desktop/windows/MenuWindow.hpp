#pragma once

#include <vector>

#include "AppWindow.hpp"
#include "GridLayout.hpp"
#include "widgets/BottomBar.hpp"
#include "widgets/Label.hpp"
#include "widgets/Rect.hpp"
#include "widgets/TopBar.hpp"

#include "utf8/UTF8.hpp"

namespace gui
{
    /// icon, description, name of application to run
    struct Tile : public Rect
    {
        Tile(UTF8 icon, std::string title, std::function<bool(Item &)> activatedCallback, unsigned int notifications = 0);
    };

    class MenuPage : public gui::GridLayout
    {
      public:
        UTF8 title;
        MenuPage(gui::Item *parent, UTF8 title, std::vector<Tile *> tiles);
    };

    class MenuWindow : public AppWindow
    {
      protected:
        // page that is currently selected by the user.
        MenuPage *mainMenu = nullptr;
        MenuPage *toolsMenu = nullptr;

      public:
        MenuWindow(app::Application *app);
        virtual ~MenuWindow();

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool onInput(const InputEvent &inputEvent) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        void switchMenu(MenuPage *page);
    };

} /* namespace gui */
