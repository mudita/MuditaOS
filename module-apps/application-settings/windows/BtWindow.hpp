#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Window.hpp"
#include <memory>
#include <BoxLayout.hpp>
#include <service-bt/messages/BtInject.hpp>


namespace gui
{

    class BtWindow : public AppWindow
    {
      protected:
        Label *send_button;

        gui::Item *addOptionLabel(const std::string &text, std::function<bool(Item &)> activatedCallback);
        BtOnOff stamp_message;

      public:
        BtWindow(app::Application *app);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        bool onInput(const InputEvent &inputEvent) override;
        void destroyInterface() override;
        void set_navigation();
    };
} // namespace gui
