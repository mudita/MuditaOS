#pragma once
#include <module-apps/windows/Dialog.hpp>
#include <Text.hpp>

namespace gui
{
    class SimSwitchWindow : public Dialog
    {
      private:
        gui::Text *password;
        void update();

      public:
        SimSwitchWindow(app::Application *app);
        ~SimSwitchWindow() override = default;
        std::string getPassword()
        {
            return password->getText();
        }
        void update(Meta &meta);
    };
} // namespace gui