#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <BoxLayout.hpp>

namespace gui
{

    class BtWindow : public AppWindow
    {
      protected:
        std::vector<gui::Item *> options;
        VBox *box;

        gui::Item *addOptionLabel(const std::string &text, std::function<bool(Item &)> activatedCallback);

      public:
        BtWindow(app::Application *app);
        virtual ~BtWindow();

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void destroyInterface() override;
        void set_navigation();
    };
} // namespace gui
