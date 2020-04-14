#pragma once

#include "AppWindow.hpp"
#include <widgets/Text.hpp>
#include <string>

namespace gui
{

    namespace name
    {
        namespace window
        {
            inline std::string new_sms = "NewSMS";
        };
    }; // namespace name

    class NewSMS_Window : public AppWindow
    {
      private:
        gui::Text *text = nullptr;

      public:
        NewSMS_Window(app::Application *app);
        virtual ~NewSMS_Window() = default;

        // virtual methods
        bool onInput(const InputEvent &inputEvent) override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

} /* namespace gui */
